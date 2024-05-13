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
	connect(this, &StartListModel::recordChanged, Application::instance(), &Application::updateRun);
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
	if (auto o_col_name = roleToName(role); o_col_name.has_value()) {
		auto col_name = o_col_name.value();
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
	else {
		return {};
	}
}

std::optional<QString> StartListModel::roleToName(Role role) const
{
	switch (role) {
	case Role::CompetitorName: return QStringLiteral("competitorName");
	case Role::Registration: return QStringLiteral("competitors.registration");
	case Role::ClassName: return QStringLiteral("classes.name");
	case Role::StartTime: return QStringLiteral("runs.starttimems");
	case Role::RunId: return QStringLiteral("runs.id");
	case Role::SiId: return QStringLiteral("runs.siid");
	case Role::CorridorTime: return QStringLiteral("runs.corridorTime");
	}
	return {};
}

QVariant StartListModel::roleValue(int row, Role role) const
{
	if (auto col = roleToColumn(role); col.has_value()) {
		return m_result.value(row, col.value());
	}
	return {};
}

std::optional<int> StartListModel::runIdToRow(int run_id) const
{
	for (auto i = 0; i < rowCount(); ++i) {
		if (roleValue(i, Role::RunId).toInt() == run_id) {
			return i;
		}
	}
	return {};
}

QVariant StartListModel::recordValue(int run_id, Role role) const
{
	if (auto o_row = runIdToRow(run_id); o_row.has_value()) {
		auto row = o_row.value();
		return roleValue(row, role);
	}
	return {};
}

void StartListModel::setRecord(int run_id, const QMap<StartListModel::Role, QVariant> &record)
{
	if (auto o_row = runIdToRow(run_id); o_row.has_value()) {
		auto row = o_row.value();
		QVariantMap chngmap;
		for (const auto &[role, val] : record.asKeyValueRange()) {
			if (auto o_col_name = roleToName(role); o_col_name.has_value()) {
				auto col_name = o_col_name.value();
				auto v = m_result.value(row, col_name);
				if (v != val) {
					chngmap[col_name] = val;
					m_result.setValue(row, col_name, val);
				}
			}
		}
		if (!chngmap.isEmpty()) {
			emit recordChanged(run_id, chngmap);
		}
	}
}

void StartListModel::onRunChanged(int run_id, const QVariant &record)
{
	if (record.isNull()) {
		if (auto o_row = runIdToRow(run_id); o_row.has_value()) {
			auto row = o_row.value();
			beginRemoveRows({}, row, row);
			m_result.rows.removeAt(row);
			endRemoveRows();
		}
	}
	else {
		auto rec = record.toMap();
		if (auto o_row = runIdToRow(run_id); o_row.has_value()) {
			auto row = o_row.value();
			for (const auto &[key, val] : rec.asKeyValueRange()) {
				m_result.setValue(row, key, val);
			}
			auto ix = createIndex(row, 0);
			emit dataChanged(ix, ix);
		}
	}
}
