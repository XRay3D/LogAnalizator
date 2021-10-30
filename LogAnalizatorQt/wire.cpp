#include "wire.h"

#include <QPainter>

static constexpr int Height { 40 };

static constexpr int U { 2 };
static constexpr int D { Height - U };
static constexpr int B { 5 };

void Wire::setData(std::vector<uint8_t> newData) {
    data_ = std::move(newData);
    update(boundingRect());
}

Wire::Wire(int wire, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , wire { wire }
    , mask(1 << wire) {
    setPos(0, wire * Height);
}

QRectF Wire::boundingRect() const {
    return QRectF { {}, QSizeF(data_.size() * B, Height) };
}

void Wire::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    painter->drawRect(boundingRect());
    painter->setPen({ Qt::red, 0.0 });
    if (data_.empty())
        return;
    bool dir = data_[0] & mask;
    for (int x {}; auto&& val : data_) {
        if (dir && val & mask) {
            painter->drawLine(QLine { { x, U }, { x += B, U } });
        } else if (!dir && !(val & mask)) {
            painter->drawLine(QLine { { x, D }, { x += B, D } });
        } else if (dir && !(val & mask)) {
            painter->drawLine(QLine { { x, U }, { x, D } });
            painter->drawLine(QLine { { x, D }, { x += B, D } });
        } else if (!dir && val & mask) {
            painter->drawLine(QLine { { x, D }, { x, U } });
            painter->drawLine(QLine { { x, U }, { x += B, U } });
        }
        dir = val & mask;
    }
}
