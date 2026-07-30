#pragma once

// HEADER FILE FOR C++/Qt DEBUGGING HELPER FUNCTIONS

#pragma warning(push, 0)

#include <QPainter>

namespace jan
{

static bool testCrosshairEnabled = true;

static void drawCrosshair(QPainter* painter, QRect& rect, QColor color)
{
  if (!testCrosshairEnabled) return;
  if (!painter) return;
  painter->save();
  painter->setPen(color);
  auto x = rect.x();
  auto y = rect.y();
  auto w = rect.width() - 1;
  auto h = rect.height() - 1;
  auto mx = x + w / 2;
  auto my = y + h / 2;
  QRect i1 = QRect(mx, y, 0, 4);
  QRect i2 = QRect(mx, y + h - 4, 0, 4);
  QRect i3 = QRect(x, my, 4, 0);
  QRect i4 = QRect(x + w - 4, my, 4, 0);
  painter->drawRect(QRect(x, y, w, h));
  painter->drawRect(i1);
  painter->drawRect(i2);
  painter->drawRect(i3);
  painter->drawRect(i4);
  painter->restore();
}

static void drawCrosshair(QPainter& painter, QRect& rect, QColor color)
{
  drawCrosshair(&painter, rect, color);
}

static void drawCrosshair(QPainter &painter, QPixmap &pixmap, QPoint& position, QColor color)
{
  if (!testCrosshairEnabled) return;
  if (!pixmap.isNull())
  {
    QRect outlineRect = QRect(position.x(), position.y(), pixmap.width(), pixmap.height());
    drawCrosshair(painter, outlineRect, color);
  }
}

static void setWireframeStyle(QWidget* widget)
{
  widget->setStyleSheet("QWidget { border: 1px solid blue; } QLabel{ border: 1px solid red } border 1px solid cyan");
}

// --- string conversion helpers

static QString toString(Qt::Key qtKey)
{
    static std::map<int, const char*> map;
#define JAN_MAP map
#include "qt_key_map.h"

    return QString(map[qtKey]);
}

static QString toString(const Qt::KeyboardModifiers& modifiers)
{
    static std::map<Qt::KeyboardModifier, const char*> map;
#define JAN_MAP map
#include "qt_keyboard_modifiers.h"

    if (modifiers == 0)
    {
        return QString("NoModifiers");
    }
    else
    {
        QString result = "";
        for (const auto& [key, value] : map)
        {
            if ((modifiers & key) != 0)
            {
                result += (result.isEmpty() ? "" : "|") + QString(value);
            }
        }
        return result;
    }
}

static QString toString(const QWidget* widget)
{
    return widget ? QString("QWidget{ %1, '%2', [@%3] }").arg(widget->metaObject()->className()).arg(widget->objectName()).arg(QString::number(reinterpret_cast<uint64_t>(widget))) : "nullptr";
}

static QString toString(const QObject* object)
{
    return object ? QString("QObject{ %1, '%2', [@%3] }").arg(object->metaObject()->className()).arg(object->objectName()).arg(QString::number(reinterpret_cast<uint64_t>(object))) : "nullptr";
}

/** Prints QKeyEvent to QString. Optional receiver can be passed in from event handler methods
*/
static QString toString(const QKeyEvent* event, const QObject* receiver = nullptr)
{
    return QString("QKeyEvent{ %1, %2, %3, Receiver: %4 }")
        .arg(event->type() == QEvent::KeyPress ? "KeyPress  " : "KeyRelease")
        .arg(jan::toString((Qt::Key)(event->key())))
        .arg(jan::toString(event->modifiers()))
        .arg(jan::toString(receiver));
}

} // end of namespace jan

#if SHMC_INCLUDED
// --- shmchat overloads ---

void shmc_println(ShmChat *shmc, QKeyEvent* keyEvent, QObject* receiver = nullptr)
{
    shmc_println(shmc, jan::toString(keyEvent, receiver).toStdString());
}
#endif

#pragma warning(pop)