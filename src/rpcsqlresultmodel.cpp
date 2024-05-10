#include "rpcsqlresultmodel.h"
#include "application.h"

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

//=========================================================
// StartListModel
//=========================================================
StartListModel::StartListModel(QObject *parent)
	: Super(parent)
{
	connect(Application::instance(), &Application::runChanged, this, &StartListModel::onRunChanged);
}

QVariant StartListModel::data(const QModelIndex &index, int role) const
{
	if (role >= Qt::UserRole) {
		return roleValue(index.row(), static_cast<Role>(role));
	}
	return {};
}

std::optional<int> StartListModel::roleToColumn(Role role) const
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
	case Role::RunId: {
		col_name = QStringLiteral("runs.id");
		break;
	}
	case SiId: {
		col_name = QStringLiteral("runs.siid");
		break;
	}
	case CorridorTime: {
		col_name = QStringLiteral("runs.corridorTime");
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
	return result_col;
}

QVariant StartListModel::roleValue(int row, Role role) const
{
	if (auto col = roleToColumn(role); col.has_value()) {
		return m_result.value(row, col.value());
	}
	return {};
}

void StartListModel::onRunChanged(int run_id, const QVariant &record)
{
	if (record.isNull()) {
		for (auto i = 0; i < rowCount(); ++i) {
			if (roleValue(i, Role::RunId).toInt() == run_id) {
				beginRemoveRows({}, i, i);
				m_result.rows.removeAt(i);
				endRemoveRows();
			}
		}
	}
	else {
		auto rec = record.toMap();
		for (auto i = 0; i < rowCount(); ++i) {
			if (roleValue(i, Role::RunId).toInt() == run_id) {
				for (const auto &[key, val] : rec.asKeyValueRange()) {
					m_result.setValue(i, key, val);
				}
				auto ix = createIndex(i, 0);
				emit dataChanged(ix, ix);
			}
		}
	}
}
