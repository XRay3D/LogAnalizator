#pragma once

#include <QGraphicsObject>

class Wire : public QGraphicsObject {
    Q_OBJECT

    std::vector<uint8_t>
        data_;
    int const wire;
    uint8_t const mask;

signals:

public:
    explicit Wire(int wire, QGraphicsItem* parent = nullptr);

    // QGraphicsItem interface
    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void setData(std::vector<uint8_t> newData);
};
