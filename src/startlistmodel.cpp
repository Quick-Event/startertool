#include "startlistmodel.h"

#include "application.h"

#include <shv/coreqt/log.h>

StartListModel::StartListModel(QObject *parent)
	: Super(parent)
	, m_roleTypes{{Role::CorridorTime, QMetaType::Type::QDateTime}}
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
		auto v = m_result.value(row, col.value());
		v = retypeValue(v, role);
		return v;
	}
	return {};
}

void StartListModel::setRoleValue(int row, Role role, const QVariant &val)
{
	if (auto o_col_name = roleToName(role); o_col_name.has_value()) {
		auto col_name = o_col_name.value();
		m_result.setValue(row, col_name, val);
		QModelIndex ix = createIndex(row, 0);
		emit dataChanged(ix, ix);
	}
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
			shvDebug() << "val:" << val.toString() << "is valid:" << val.isValid() << "is null:" << val.isNull() << "type:" << val.typeName();
			auto v1 = roleValue(row, role);
			setRoleValue(row, role, val);
			auto v2 = roleValue(row, role);
			shvDebug() << "v1 val:" << v1.toString() << "is valid:" << v1.isValid() << "is null:" << v1.isNull() << "type:" << v1.typeName();
			shvDebug() << "v2 val:" << v2.toString() << "is valid:" << v2.isValid() << "is null:" << v2.isNull() << "type:" << v2.typeName();
			if ((!v1.isValid() || v1.isNull()) && (!v2.isValid() || v2.isNull())) {
				continue;
			}
			if (v1 != v2) {
				if (auto colname = roleToName(role); colname.has_value()) {
					shvDebug() << "chng:" << v2.toString() << v2.typeName();
					chngmap[colname.value()] = v2;
				}
			}
		}
		if (!chngmap.isEmpty()) {
			emit recordChanged(run_id, chngmap);
		}
	}
}

QVariant StartListModel::retypeValue(const QVariant &val, Role role) const
{
	if (!val.isValid()) {
		return val;
	}
	if (auto mt = m_roleTypes.value(role); mt > 0) {
		switch (mt) {
		case QMetaType::Type::QDateTime: {
			auto dt = val.toDateTime();
			if (!dt.isValid()) {
				return {};
			}
			return dt;
		}
		default:
			break;
		}
	}
	return val;
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
