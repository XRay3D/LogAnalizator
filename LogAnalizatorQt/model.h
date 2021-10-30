#pragma once

#include <QAbstractTableModel>

class Model : public QAbstractTableModel {
    Q_OBJECT

    std::vector<uint8_t> data_;

public:
    explicit Model(QObject* parent = nullptr);

    void setData(std::vector<uint8_t>&& data);

    // QAbstractItemModel interface
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QVariant data(const QModelIndex& index, int role) const override;


    Qt::ItemFlags flags(const QModelIndex &index) const override;
};
