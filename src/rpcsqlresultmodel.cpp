#include "rpcsqlresultmodel.h"

#include <shv/coreqt/log.h>
#include <shv/coreqt/rpc.h>

RpcSqlResultModel::RpcSqlResultModel(QObject *parent)
	: Super{parent}
{

}

int RpcSqlResultModel::rowCount(const QModelIndex &) const
{
	return m_result.rows.count();
}

int RpcSqlResultModel::columnCount(const QModelIndex &) const
{
	return m_result.fields.count();
}

QVariant RpcSqlResultModel::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DisplayRole) {
		return m_result.value(index.row(), index.column());
	}
	return {};
}

QVariant RpcSqlResultModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal) {
		if (role == Qt::DisplayRole) {
			return m_result.fields.value(section).name;
		}
	}
	return {};
}

void RpcSqlResultModel::setResult(const Result &result)
{
	beginResetModel();
	m_result = result;
	endResetModel();
}

