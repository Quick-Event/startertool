#include "startlistmodel.h"

#include "application.h"

#include <shv/coreqt/log.h>

StartListModel::StartListModel(QObject *parent)
	: Super(parent)
	, m_roleTypes{{Role::CorridorTime, QMetaType::Type::QDateTime}}
{
	auto *app = Application::instance();
	connect(app, &Application::runChanged, this, &StartListModel::onRunChanged);
	connect(this, &StartListModel::recordChanged, Application::instance(), &Application::updateRun);
	connect(app, &Application::currentTimeChanged, this, [this](const QDateTime &current_time) {
		if (m_corridorTime.time().second() != current_time.time().second()) {
			m_corridorTime = current_time;
			auto line_time = current_time.addSecs(-60);
			//shvInfo() << "UPDATE===================" << m_corridorTime;
			//ui->tableView->update();
			for (auto row = 0; row < rowCount(); ++row) {
				auto start = roleValue(row, Role::StartTime).toDateTime();
				auto diff = start.secsTo(line_time);
				if (diff >= 0 && diff < (60 * 4)) {
					auto ix = index(row, 0);
					emit dataChanged(ix, ix);
				}
			}
		}
	});
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
	case Role::RunId: return QStringLiteral("runs.id");
	case Role::CompetitorName: return QStringLiteral("competitorName");
	case Role::Registration: return QStringLiteral("competitors.registration");
	case Role::ClassName: return QStringLiteral("classes.name");
	case Role::StartTime: return QStringLiteral("runs.starttimems");
	case Role::SiId: return QStringLiteral("runs.siid");
	case Role::CorridorTime: return QStringLiteral("runs.corridorTime");
	case Role::IsSelectedRow: throw std::runtime_error("IsSelectedRow role has not column");
	break;
	}
	return {};
}

QVariant StartListModel::roleValue(int row, Role role) const
{
	if (role == Role::IsSelectedRow) {
		if (m_selectedRow.has_value()) {
			return m_selectedRow.value() == row;
		}
		return false;
	}
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

void StartListModel::setSelectedRow(std::optional<int> row)
{
	if (row == m_selectedRow)
		return;
	if (m_selectedRow.has_value()) {
		auto ix = index(m_selectedRow.value(), 0);
		emit dataChanged(ix, ix);
	}
	m_selectedRow = row;
	if (m_selectedRow.has_value()) {
		auto ix = index(m_selectedRow.value(), 0);
		emit dataChanged(ix, ix);
	}
}

QList<int> StartListModel::fullTextSearch(const QString &txt) const
{
	if (txt.length() < 3)
		return {};
	QMap<int, int> ret;
	for (auto i = 0; i < rowCount(); ++i) {
		for (auto role : {CompetitorName, Registration, SiId}) {
			auto s = roleValue(i, role).toString();
			if (s.contains(txt, Qt::CaseInsensitive)) {
				ret[i] = role;
			}
		}
	}
	return ret.keys();
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
