#include <limits>

#include "rrtstar.h"

#define OBSTACLE_THRESHOLD 200

RRTNode::RRTNode(POS2D pos) {
    m_pos = pos;
    m_cost = 0.0;
    mp_parent = NULL;
}

bool RRTNode::operator==(const RRTNode &other) {
    return m_pos==other.m_pos;
}

Path::Path(POS2D start, POS2D goal) {
    m_start = start;
    m_goal = goal;
    m_cost = 0.0;
}

Path::~Path() {
    m_cost = 0.0;
}

RRTstar::RRTstar( int width, int height, int segment_length ) {

    _sampling_width = width;
    _sampling_height = height;
    _segment_length = segment_length;
    _p_root = NULL;

    _p_kd_tree = new KDTree2D( std::ptr_fun(tac) );

    _range = (_sampling_width > _sampling_height) ? _sampling_width:_sampling_height;
    _ball_radius = _range;
    _obs_check_resolution = 1;
    _current_iteration = 0;
    _segment_length = segment_length;

    _theta = 5;

    _pp_map_info = new int*[_sampling_width];
    for(int i=0;i<_sampling_width;i++) {
        _pp_map_info[i] = new int[_sampling_height];
        for(int j=0;j<_sampling_height;j++)
        {
            _pp_map_info[i][j] = 255;
        }
    }

    _nodes.clear();
}

RRTstar::~RRTstar() {
    if(_p_kd_tree) {
        delete _p_kd_tree;
        _p_kd_tree = NULL;
    }
}

RRTNode* RRTstar::init( POS2D start, POS2D goal, COST_FUNC_PTR p_func, double** pp_cost_distribution ) {
    if( _p_root ) {
        delete _p_root;
        _p_root = NULL;
    }
    _start = start;
    _goal = goal;
    _p_cost_func = p_func;
    _pp_cost_distribution = pp_cost_distribution;

    KDNode2D root( start );

    _p_root = new RRTNode( start );
    _nodes.push_back(_p_root);
    root.setRRTNode(_p_root);

    _p_kd_tree->insert( root );
    _current_iteration = 0;

    return _p_root;
}

void RRTstar::load_map( int** pp_map ) {
    for(int i=0;i<_sampling_width;i++)
    {
        for(int j=0;j<_sampling_height;j++)
        {
            _pp_map_info[i][j] = pp_map[i][j];
        }
    }
}

POS2D RRTstar::_sampling() {
    double x = rand();
    double y = rand();
    x = x * ((double)(_sampling_width)/RAND_MAX);
    y = y * ((double)(_sampling_height)/RAND_MAX);

    POS2D m(x,y);
    return m;
}

POS2D RRTstar::_steer( POS2D pos_a, POS2D pos_b ) {
    POS2D new_pos( pos_a[0], pos_a[1] );
    double delta[2];
    delta[0] = pos_a[0] - pos_b[0];
    delta[1] = pos_a[1] - pos_b[1];
    double delta_len = sqrt(delta[0]*delta[0]+delta[1]*delta[1]);

    if (delta_len > _segment_length) {
        double scale = _segment_length / delta_len;
        delta[0] = delta[0] * scale;
        delta[1] = delta[1] * scale;

        new_pos.setX( pos_b[0]+delta[0] );
        new_pos.setY( pos_b[1]+delta[1] );
    }
    return new_pos;
}

bool RRTstar::_is_in_obstacle( POS2D pos ) {
    int x = (int)pos[0];
    int y = (int)pos[1];
    if( _pp_map_info[x][y] < 255 ) {
        return true;
    }
    return false;
}


bool RRTstar::_is_obstacle_free( POS2D pos_a, POS2D pos_b ) {
    if (pos_a == pos_b) {
        return true;
    }
    int x_dist = pos_a[0] - pos_b[0];
    int y_dist = pos_a[1] - pos_b[1];
    if ( fabs( x_dist ) > fabs( y_dist ) ) {
        double k = (double)y_dist/ x_dist;
        int start_x = 0, end_x = 0, start_y = 0;
        if ( pos_a[0] < pos_b[0] ) {
            start_x = pos_a[0];
            end_x   = pos_b[0];
            start_y = pos_a[1];
        }
        else {
            start_x = pos_b[0];
            end_x   = pos_a[0];
            start_y = pos_b[1];
        }
        for ( int coord_x = start_x; coord_x < end_x + _obs_check_resolution; coord_x+=_obs_check_resolution ) {
            int coordY = (int)(k*(coord_x-start_x)+start_y);
            if ( coordY >= _sampling_height || coord_x >= _sampling_width ) break;
            if ( _pp_map_info[coord_x][coordY] < OBSTACLE_THRESHOLD ) {
                return false;
            }
        }
    }
    else {
        double k = (double)x_dist/ y_dist;
        int start_y = 0, end_y = 0, start_x =0;
        if ( pos_a[1] < pos_b[1] ) {
            start_y = pos_a[1];
            end_y   = pos_b[1];
            start_x = pos_a[0];
        }
        else {
            start_y = pos_b[1];
            end_y   = pos_a[1];
            start_x = pos_b[0];
        }
        for ( int coordY = start_y; coordY < end_y + _obs_check_resolution; coordY+=_obs_check_resolution ) {
            int coordX = (int)(k*(coordY-start_y)+start_x);
            if ( coordY >= _sampling_height || coordX >= _sampling_width ) break;
            if ( _pp_map_info[coordX][coordY] < OBSTACLE_THRESHOLD ) {
                return false;
            }
        }
    }
    return true;
}

void RRTstar::extend() {
    bool node_inserted = false;
    while( false==node_inserted ) {
        POS2D rnd_pos = _sampling();
        KDNode2D nearest_node = _find_nearest( rnd_pos );

        POS2D new_pos = _steer( rnd_pos, nearest_node );

        if( true == _contains(new_pos) ) {
            continue;
        }
        if( true == _is_in_obstacle( new_pos ) ) {
            continue;
        }

        if( true == _is_obstacle_free( nearest_node, new_pos ) ) {
            std::list<KDNode2D> near_list = _find_near( new_pos );
            KDNode2D new_node( new_pos );

            // create new node
            RRTNode * p_new_rnode = _create_new_node( new_pos );
            new_node.setRRTNode( p_new_rnode );

            _p_kd_tree->insert( new_node );
            node_inserted = true;

            RRTNode* p_nearest_rnode = nearest_node.getRRTNode();
            std::list<RRTNode*> near_rnodes;
            near_rnodes.clear();
            for( std::list<KDNode2D>::iterator itr = near_list.begin();
                itr != near_list.end(); itr++ ) {
                KDNode2D kd_node = (*itr);
                RRTNode* p_near_rnode = kd_node.getRRTNode();
                near_rnodes.push_back( p_near_rnode );
            }

            // attach new node to reference trees
            _attach_new_node( p_new_rnode, p_nearest_rnode, near_rnodes );
            // rewire near nodes of reference trees
            _rewire_near_nodes( p_new_rnode, near_rnodes );
        }
    }
    _current_iteration++;
}

KDNode2D RRTstar::_find_nearest( POS2D pos ) {
    KDNode2D node( pos );

    std::pair<KDTree2D::const_iterator,double> found = _p_kd_tree->find_nearest( node );
    KDNode2D near_node = *found.first;
    return near_node;
}

std::list<KDNode2D> RRTstar::_find_near(POS2D pos) {
    std::list<KDNode2D> near_list;
    KDNode2D node(pos);

    int num_vertices = _p_kd_tree->size();
    int num_dimensions = 2;
    _ball_radius = _range * pow( log((double)(num_vertices + 1.0))/((double)(num_vertices + 1.0)), 1.0/((double)num_dimensions) );

    _p_kd_tree->find_within_range( node, _ball_radius, std::back_inserter( near_list ) );

    return near_list;
}


bool RRTstar::_contains( POS2D pos )
{
    if(_p_kd_tree) {
        KDNode2D node( pos[0], pos[1] );
        KDTree2D::const_iterator it = _p_kd_tree->find(node);
        if( it!=_p_kd_tree->end() ) {
            return true;
        }
        else {
            return false;
        }
    }
    return false;
}

double RRTstar::_calculate_cost( POS2D& pos_a, POS2D& pos_b ) {
    int dimension[2];
    dimension[0] = _sampling_width;
    dimension[1] = _sampling_height;
    return _p_cost_func(pos_a, pos_b, _pp_cost_distribution, dimension);
}

RRTNode* RRTstar::_create_new_node(POS2D pos) {
    RRTNode * pNode = new RRTNode(pos);
    _nodes.push_back(pNode);

    return pNode;
}

bool RRTstar::_remove_edge(RRTNode* p_node_parent, RRTNode*  p_node_child) {
    if( p_node_parent==NULL ) {
        return false;
    }

    p_node_child->mp_parent = NULL;
    bool removed = false;
    for( std::list<RRTNode*>::iterator it=p_node_parent->m_child_nodes.begin();it!=p_node_parent->m_child_nodes.end();it++ ) {
        RRTNode* p_current = (RRTNode*)(*it);
        if ( p_current == p_node_child || p_current->m_pos==p_node_child->m_pos ) {
            p_current->mp_parent = NULL;
            it = p_node_parent->m_child_nodes.erase(it);
            removed = true;
        }
    }
    return removed;
}

bool RRTstar::_has_edge(RRTNode* p_node_parent, RRTNode* p_node_child) {
    if ( p_node_parent == NULL || p_node_child == NULL ) {
        return false;
    }
    for( std::list<RRTNode*>::iterator it=p_node_parent->m_child_nodes.begin();it!=p_node_parent->m_child_nodes.end();it++ ) {
        RRTNode* p_curr_node = (*it);
        if( p_curr_node == p_node_child ) {
            return true;
        }
    }
    /*
    if (pNode_p == pNode_c->mpParent)
        return true;
    */
    return false;
}

bool RRTstar::_add_edge( RRTNode* p_node_parent, RRTNode* p_node_child ) {
    if( p_node_parent == NULL || p_node_child == NULL || p_node_parent == p_node_child ) {
        return false;
    }
    if ( p_node_parent->m_pos == p_node_child->m_pos ) {
        return false;
    }
    if ( true == _has_edge( p_node_parent, p_node_child ) ) {
        p_node_child->mp_parent = p_node_parent;
    }
    else {
        p_node_parent->m_child_nodes.push_back( p_node_child );
        p_node_child->mp_parent = p_node_parent;
    }
    p_node_child->m_child_nodes.unique();

    return true;
}


std::list<RRTNode*> RRTstar::_find_all_children( RRTNode* p_node ) {
    int level = 0;
    bool finished = false;
    std::list<RRTNode*> child_list;

    std::list<RRTNode*> current_level_nodes;
    current_level_nodes.push_back( p_node );
    while( false==finished ) {
        std::list<RRTNode*> current_level_children;
        int child_list_num = child_list.size();

        for( std::list<RRTNode*>::iterator it=current_level_nodes.begin(); it!=current_level_nodes.end(); it++ ) {
            RRTNode* pCurrentNode = (*it);
            for( std::list<RRTNode*>::iterator itc=pCurrentNode->m_child_nodes.begin(); itc!=pCurrentNode->m_child_nodes.end();itc++ ) {
                RRTNode *p_child_node= (*itc);
                if(p_child_node) {
                    current_level_children.push_back(p_child_node);
                    child_list.push_back(p_child_node);
                }
            }
        }

        child_list.unique();
        current_level_children.unique();

        if (current_level_children.size()==0) {
            finished = true;
        }
        else if (child_list.size()==child_list_num) {
            finished = true;
        }
        else {
            current_level_nodes.clear();
            for( std::list<RRTNode*>::iterator itt=current_level_children.begin();itt!=current_level_children.end();itt++ ) {
                RRTNode * pTempNode = (*itt);
                if( pTempNode ) {
                    current_level_nodes.push_back( pTempNode );
                }
            }
            level +=1;
        }

        if(level>100) {
            break;
        }
    }
    child_list.unique();
    return child_list;
}


RRTNode* RRTstar::_find_ancestor(RRTNode* p_node) {
    return get_ancestor( p_node );
}

Path* RRTstar::find_path() {
    Path* p_new_path = new Path( _start, _goal );

    std::list<RRTNode*> node_list;

    RRTNode * p_first_node = NULL;
    double delta_cost = 0.0;
    _get_closet_to_goal( p_first_node, delta_cost );

    if( p_first_node != NULL ) {
        get_parent_node_list( p_first_node, node_list );
        for( std::list<RRTNode*>::reverse_iterator rit=node_list.rbegin();
            rit!=node_list.rend(); ++rit ) {
            RRTNode* p_node = (*rit);
            p_new_path->m_way_points.push_back( p_node->m_pos );
        }
        p_new_path->m_way_points.push_back(_goal);

        p_new_path->m_cost = p_first_node->m_cost + delta_cost;
    }

    return p_new_path;
}


void RRTstar::_attach_new_node(RRTNode* p_node_new, RRTNode* p_nearest_node, std::list<RRTNode*> near_nodes) {
    double min_new_node_cost = p_nearest_node->m_cost + _calculate_cost(p_nearest_node->m_pos, p_node_new->m_pos);
    RRTNode* p_min_node = p_nearest_node;

    for(std::list<RRTNode*>::iterator it=near_nodes.begin();it!=near_nodes.end();it++) {
        RRTNode* p_near_node = *it;
        if ( true == _is_obstacle_free( p_near_node->m_pos, p_node_new->m_pos ) ) {
            double new_cost = p_near_node->m_cost + _calculate_cost( p_near_node->m_pos, p_node_new->m_pos );
            if ( new_cost < min_new_node_cost ) {
                p_min_node = p_near_node;
                min_new_node_cost = new_cost;
            }
        }
    }

    bool added = _add_edge( p_min_node, p_node_new );
    if( added ) {
        p_node_new->m_cost = min_new_node_cost;
    }

}

void RRTstar::_rewire_near_nodes(RRTNode* p_node_new, std::list<RRTNode*> near_nodes) {
    for( std::list<RRTNode*>::iterator it=near_nodes.begin(); it!=near_nodes.end(); it++ ) {
        RRTNode * p_near_node = (*it);

        if(p_near_node->m_pos ==p_node_new->m_pos ||  p_near_node->m_pos==_p_root->m_pos || p_node_new->mp_parent->m_pos==p_near_node->m_pos) {
            continue;
        }

        if( true == _is_obstacle_free( p_node_new->m_pos, p_near_node->m_pos ) ) {
            double temp_cost_from_new_node = p_node_new->m_cost + _calculate_cost( p_node_new->m_pos, p_near_node->m_pos );
            if( temp_cost_from_new_node < p_near_node->m_cost ) {
                double delta_cost = p_near_node->m_cost - temp_cost_from_new_node;
                RRTNode * p_parent_node = p_near_node->mp_parent;
                bool removed = _remove_edge(p_parent_node, p_near_node);
                if(removed) {
                    bool added = _add_edge(p_node_new, p_near_node);
                    if( added ) {
                        p_near_node->m_cost = temp_cost_from_new_node;
                        _update_cost_to_children(p_near_node, delta_cost);
                    }
                }
                else {
                    std::cout << " Failed in removing " << std::endl;
                }
            }
        }
    }
}

void RRTstar::_update_cost_to_children( RRTNode* p_node, double delta_cost ) {
    std::list<RRTNode*> child_list = _find_all_children( p_node );
    for( std::list<RRTNode*>::iterator it = child_list.begin(); it != child_list.end();it++ ) {
        RRTNode* p_child_node = (*it);
        if( p_child_node ) {
            p_child_node->m_cost -= delta_cost;
        }
    }
}

bool RRTstar::_get_closet_to_goal( RRTNode*& p_node_closet_to_goal, double& delta_cost ) {
    bool found = false;

    std::list<KDNode2D> near_nodes = _find_near( _goal );
    double min_total_cost = std::numeric_limits<double>::max();

    for(std::list<KDNode2D>::iterator it=near_nodes.begin();
        it!=near_nodes.end();it++) {
        KDNode2D kd_node = (*it);
        RRTNode* p_node = kd_node.getRRTNode();
        double new_delta_cost = _calculate_cost(p_node->m_pos, _goal);
        double new_total_cost= p_node->m_cost + new_delta_cost;
        if (new_total_cost < min_total_cost) {
            min_total_cost = new_total_cost;
            p_node_closet_to_goal = p_node;
            delta_cost = new_delta_cost;
            found = true;
        }
    }
    return found;
}

