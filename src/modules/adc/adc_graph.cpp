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
#include "adc_graph.h"

ADCGraph::ADCGraph(QWidget* parent)
  : QGraphicsView(parent)
{
  this->setRenderHints(QPainter::Antialiasing);
  this->lastX = this->lastY = 0;
  this->graph = new QGraphicsScene(this);
  this->graph->setBackgroundBrush(QBrush(Qt::black));
  this->setScene(this->graph);
  this->zoom = 1;
}

void
ADCGraph::drawLineTo(double x, double y)
{
  this->graph->addLine(this->lastX, this->lastY, x * this->zoom, y * this->zoom,
                       QPen(Qt::green));
  QRectF rect(this->graph->sceneRect());
  rect.moveRight(this->lastX - (this->graph->width()));
  rect.moveBottom(this->lastY - (this->graph->height()));
  this->graph->setSceneRect(rect);
  this->lastX = x * this->zoom;
  this->lastY = y * this->zoom;
}

void
ADCGraph::setZoom(int x)
{
  this->graph->clear();
  this->zoom = x;
}
