#ifndef RRTSTAR_VIZ_H_
#define RRTSTAR_VIZ_H_

#include <QLabel>

#include "rrtstar.h"
#include "path_planning_info.h"

class RRTstarViz : public QLabel
{
    Q_OBJECT
public:
    explicit RRTstarViz(QWidget *parent = 0);
    void setTree(RRTstar* p_tree);

    PathPlanningInfo m_PPInfo;
signals:
    
public slots:

private:
    RRTstar* mp_tree;

private slots:
    void paintEvent(QPaintEvent * e);


    
};

#endif // RRTSTAR_VIZ_H_
