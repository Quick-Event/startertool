#include "rpcsqlresultmodel.h"

#include <shv/coreqt/log.h>

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

//=========================================================
// StartListModel
//=========================================================
StartListModel::StartListModel(QObject *parent)
	: Super(parent)
{
}

QVariant StartListModel::data(const QModelIndex &index, int role) const
{
	if (role >= Qt::UserRole) {
		return roleValue(index.row(), static_cast<Role>(role));
	}
	return {};
}

QVariant StartListModel::roleValue(int row, Role role) const
{
	QString col_name;
	switch (role) {
	case Role::CompetitorName: {
		col_name = QStringLiteral("competitorName");
		break;
	}
	case Role::Registration: {
		col_name = QStringLiteral("competitors.registration");
		break;
	}
	case Role::ClassName: {
		col_name = QStringLiteral("classes.name");
		break;
	}
	case Role::StartTime: {
		col_name = QStringLiteral("runs.starttimems");
		break;
	}
	}
	if (!m_nameToIndex.contains(col_name)) {
		if (auto ix = m_result.columnIndex(col_name); ix.has_value()) {
			m_nameToIndex[col_name] = ix.value();
		}
		else {
			shvWarning() << "Unknown column name:" << col_name;
			m_nameToIndex[col_name] = -1;
		}
	}
	auto result_col = m_nameToIndex.value(col_name, -1);
	if (result_col < 0) {
		return {};
	}
	return m_result.value(row, result_col);
}
