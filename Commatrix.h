#ifndef COMMATRIX_H
#define COMMATRIX_H

#include <qqml.h>
#include <QAbstractTableModel>
#include <opencv2/opencv.hpp>
#include "barstrucs.h"

class Commatrix : public QAbstractTableModel
{
    Q_OBJECT
    QML_ELEMENT

	bc::barvector* elems;
    int wid, hei;
    cv::Mat mat;

public:
    explicit Commatrix(QObject *parent = nullptr);

    // Header:
//    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	void setMatrix(cv::Mat &matrix);
	void setElems(bc::barvector *els, const cv::Size &s);
private:
};

#endif // COMMATRIX_H
