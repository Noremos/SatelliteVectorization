#ifndef TREEMODEL_H
#define TREEMODEL_H

#include <qqml.h>
#include <QAbstractItemModel>
#include <QObject>
//#include <TreeItem.h>
#include "barclasses.h"

class TreeNode
{
public:
	TreeNode(bc::bline *node)
	{
		this->node = node;
		if (node != nullptr) {
			name = QString::number(node->start) + " " + QString::number(node->len());
		} else
			name = "NaN";
	}
	void setParrent(TreeNode *parent)
	{
		this->parent = parent;
		row = parent->childs.size();
		parent->childs.push_back(this);
	}
	bc::bline* node;
	QString name;
	QString desct;
	TreeNode *parent = nullptr;
	QVector<TreeNode*> childs;
	bool visible = true;
	int row;
	~TreeNode()
	{
		if (parent != nullptr)
			parent->childs[row] = nullptr;

		for (int i = 0; i < childs.size(); ++i)
		{
			delete childs[i];
			childs[i] = nullptr;
		}
	}
};

class TreeModel : public QAbstractItemModel
{
	Q_OBJECT
	QML_ELEMENT

public:
	enum TreeModelRoles
	{
		BarName = Qt::UserRole + 1,
		BarVisible
	};
	Q_ENUM(TreeModelRoles)
	explicit TreeModel( QObject *parent = nullptr);
	// QAbstractItemModel interface

	QModelIndex index(int row, int column, const QModelIndex &parent) const override;
	QModelIndex parent(const QModelIndex &child) const override;
	int rowCount(const QModelIndex &parent) const override;
	int columnCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

	TreeNode *NameRoot = nullptr;
	TreeNode *barRoot = nullptr;
	TreeNode *barOther = nullptr;
	~TreeModel();


	Qt::ItemFlags flags(const QModelIndex &index) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	void setGraph(bc::BarRoot *item);
	 QHash<int, QByteArray> roleNames() const override
	 {
		 QHash<int, QByteArray> roles;
		 roles[BarName] = "name";
		 roles[BarVisible] = "visible";
		 return roles;
	 }
//	 Q_INVOKABLE void *getNodeRef(QModelIndex index);

	 // QAbstractItemModel interface
public:
	bool hasChildren(const QModelIndex &parent) const override;
	QMap<int, QVariant> itemData(const QModelIndex &index) const override;

	// QAbstractItemModel interface
public:
	//	bool removeRows(int row, int count, const QModelIndex &parent) override;
	void SetSecondGraph(bc::BarRoot *item);
};

#endif // TREEMODEL_H
