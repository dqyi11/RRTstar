#ifndef PATHPLANNINGINFO_H_
#define PATHPLANNINGINFO_H_

#include <QString>
#include <QPoint>
#include <QJsonObject>
#include <list>
#include <vector>
#include <qDebug>
#include <math.h>

#include "rrtstar.h"

class PathPlanningInfo {
public:
    PathPlanningInfo();

    bool get_obstacle_info( int**& pp_obstacle_info );
    bool get_cost_distribution( double**& pp_cost_distribution );

    bool get_pix_info( QString filename, double**& pp_pix_info );
    bool get_pix_info( QString filename, int**& pp_pix_info );
    void init_func_param();

    bool save_to_file( QString filename );
    bool load_from_file( QString filename );

    void read( const QJsonObject &json );
    void write( QJsonObject &json ) const;

    void load_path( Path* path );
    bool export_path( QString filename );

    static double calc_dist( POS2D pos_a, POS2D pos_b, double** pp_distribution, int* p_dimension ) {
        double dist = 0.0;
        if (pos_a == pos_b) {
            return dist;
        }
        double delta_x = fabs(pos_a[0]-pos_b[0]);
        double delta_y = fabs(pos_a[1]-pos_b[1]);
        dist = sqrt(delta_x*delta_x+delta_y*delta_y);
        //dist = (delta_x*delta_x+delta_y*delta_y);
        if(dist < 0.0) {
            qWarning() << "Dist negative " << dist ;
        }
        return dist;
    }

    static double calc_cost( POS2D pos_a, POS2D pos_b, double** pp_distribution, int* p_dimension ) {
        double cost = 0.0;
        if ( pos_a == pos_b ) {
            return cost;
        }
        if( pp_distribution == NULL ){
            return cost;
        }

        int width = p_dimension[0];
        int height = p_dimension[1];

        double x_dist = pos_a[0] - pos_b[0];
        double y_dist = pos_a[1] - pos_b[1];
        if ( fabs( x_dist ) > fabs( y_dist ) ) {
            int start_x = 0, end_x = 0, start_y = 0, end_y = 0;
            double k = y_dist / x_dist;
            if ( pos_a[0] < pos_b[0] ) {
                start_x = (int)floor( pos_a[0] );
                end_x   = (int)floor( pos_b[0] );
                start_y = (int)floor( pos_a[1] );
                end_y   = (int)floor( pos_b[1]) ;
            }
            else {
                start_x = (int)floor( pos_b[0] );
                end_x   = (int)floor( pos_a[0] );
                start_y = (int)floor( pos_b[1] );
                end_y   = (int)floor( pos_a[1] );
            }
            for( int coord_x = start_x; coord_x < end_x; coord_x++ ) {
                int coord_y = (int)floor( k*( coord_x-start_x )+start_y );
                if ( coord_x < 0 || coord_x >= width || coord_y < 0 || coord_y >= height ) {
                    continue;
                }
                double fitness_val = pp_distribution[coord_x][coord_y];
                if( fitness_val < 0 ) {
                    qWarning() << "Cost negative " << fitness_val;
                }
                cost += fitness_val/255.0;
            }
        }
        else {
            int start_y = 0, end_y = 0, start_x = 0, end_x = 0;
            double k = x_dist / y_dist;
            if ( pos_a[0] < pos_b[0] ) {
                start_y = (int)floor( pos_a[1] );
                end_y   = (int)floor( pos_b[1] );
                start_x = (int)floor( pos_a[0] );
                end_x   = (int)floor( pos_b[0] );
            }
            else {
                start_y = (int)floor( pos_b[1] );
                end_y   = (int)floor( pos_a[1] );
                start_x = (int)floor( pos_b[0] );
                end_x   = (int)floor( pos_a[0] );
            }
            for( int coord_y = start_y; coord_y < end_y; coord_y++ ) {
                int coord_x = (int)floor( k*(coord_y-start_y)+start_x );
                if ( coord_x < 0 || coord_x >= width || coord_y < 0 || coord_y >= height ) {
                    continue;
                }
                double fitness_val = (double)pp_distribution[coord_x][coord_y];
                if( fitness_val < 0 ) {
                    qWarning() << "Cost negative " << fitness_val;
                }
                cost += fitness_val/255.0;
            }
        }

        return cost;
    }

    /* Member variables */
    QString m_info_filename;
    QString m_map_filename;
    QString m_map_fullpath;
    int m_map_width;
    int m_map_height;

    QPoint m_start;
    QPoint m_goal;

    QString m_paths_output;
    bool m_min_dist_enabled;
    QString m_objective_file;

    COST_FUNC_PTR mp_func;
    double** mCostDistribution;

    int m_max_iteration_num;
    double m_segment_length;

    Path* mp_found_path;
};

#endif //  PATHPLANNINGINFO_H_
