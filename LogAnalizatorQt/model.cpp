#include "model.h"

#include <QPainter>
#include <QPixmap>

//QSize size { 32, 32 };
//QPixmap pixmaps[] { QPixmap { size }, QPixmap { size }, QPixmap { size }, QPixmap { size } };

Model::Model(QObject* parent)
    : QAbstractTableModel(parent) {
    //    for (auto&& pixmap : pixmaps) {
    //        pixmap.fill(Qt::white);
    //    }
    //    {
    //        QPainter painter(pixmaps + 0); //0
    //        painter.setPen({ Qt::black, 2 });
    //        painter.drawLine(0, 30, 32, 30);
    //    }
    //    {
    //        QPainter painter(pixmaps + 1);
    //        painter.setPen({ Qt::black, 2 });
    //        painter.drawLine(0, 30, 32, 0);
    //    }
    //    {
    //        QPainter painter(pixmaps + 2);
    //        painter.setPen({ Qt::black, 2 });
    //        painter.drawLine(0, 30, 32, 0);
    //    }
    //    {
    //        QPainter painter(pixmaps + 3); //1
    //        painter.setPen({ Qt::black, 2 });
    //        painter.drawLine(0, 2, 32, 2);
    //    }
}

void Model::setData(std::vector<uint8_t>&& data) {
    /*  */ if (data.size() > data_.size()) {
        beginInsertColumns({}, data_.size(), data.size() - 1);
        data_ = std::move(data);
        endInsertColumns();
    } else if (data.size() < data_.size()) {
        beginRemoveColumns({}, data.size(), data_.size() - 1);
        data_ = std::move(data);
        endRemoveColumns();
    } else if (data.size() == data_.size()) {
        data_ = std::move(data);
        emit dataChanged(index(0, 0), index(7, data_.size() - 1));
    }
}

int Model::rowCount(const QModelIndex& parent) const {
    return 8;
}

int Model::columnCount(const QModelIndex& parent) const {
    return data_.size();
}

QVariant Model::data(const QModelIndex& index, int role) const {

    if (role == Qt::CheckStateRole)
        return (data_[index.column()] & (1 << index.row())) ? Qt::Checked : Qt::Unchecked;
    return {};
}

Qt::ItemFlags Model::flags(const QModelIndex& index) const {
    return QAbstractTableModel::flags(index);
}
