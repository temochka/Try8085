/*
  Copyright (C) 2009, 2016  Artem Chistyakov <chistyakov.artem@gmail.com>
  Copyright (C) 2009  Daniil Zapyatoy <zapper.kb@gmail.com>

  This file is part of Try8085.

  Try8085 is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Try8085 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Try8085.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef ACP_GRAPH_H
#define ACP_GRAPH_H

#include <QGraphicsScene>
#include <QGraphicsView>

class ADCGraph : public QGraphicsView
{
  Q_OBJECT
public:
  ADCGraph(QWidget* parent = 0);
  void drawLineTo(double x, double y);
public slots:
  void setZoom(int x);

private:
  QGraphicsScene* graph;
  double lastX;
  double lastY;
  int zoom;
};

#endif // ACP_GRAPH_H
