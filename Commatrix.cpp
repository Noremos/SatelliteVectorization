#include "Commatrix.h"
Commatrix::Commatrix(QObject *parent)
    : QAbstractTableModel(parent)
{
    mat = cv::Mat(0, 0, CV_8UC1);
}

//QVariant Commatrix::headerData(int section, Qt::Orientation orientation, int role) const
//{
//    // FIXME: Implement me!
//    return QVariant();
//}

int Commatrix::rowCount(const QModelIndex &parent) const
{
    return hei;
}

int Commatrix::columnCount(const QModelIndex &parent) const
{
    return wid;
}

QHash<int, QByteArray> Commatrix::roleNames() const
{
    return { {Qt::DisplayRole, "display"} };
}



QVariant Commatrix::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= hei || index.column() < 0 || index.column() >= wid)
        return QVariant();

    if (role == Qt::DisplayRole) {
        //        uchar u = mat.at(index.row(), index.column());
		uchar ed =0;
		for (const auto &p : (*elems)) {
			if (p.getX() == index.column() && p.getY() == index.row())
				ed = p.value;
        }
        return QVariant(ed);
    }
    else
        return QVariant();

}

void Commatrix::setMatrix(cv::Mat &matrix)
{
    beginInsertRows(QModelIndex(),0,matrix.rows);
    this->mat = matrix;
    endInsertRows();
}

void Commatrix::setElems(bc::barvector *els,const cv::Size& s)
{
    this->hei = s.height;
    this->wid = s.width;
    beginInsertRows(QModelIndex(), 0, hei);
    this->elems = els;
    endInsertRows();
}
