#include "startlistmodel.h"

#include "application.h"

#include <shv/coreqt/log.h>
#include <shv/coreqt/rpc.h>

StartListModel::StartListModel(QObject *parent)
	: Super(parent)
	, m_roleTypes{{Role::CorridorTime, QMetaType::Type::QDateTime}}
{
	auto *app = Application::instance();
	connect(app, &Application::runChanged, this, &StartListModel::applyRemoteRecordChanges);
	connect(this, &StartListModel::localRecordUpdated, Application::instance(), &Application::updateRun);
	connect(app, &Application::currentTimeChanged, this, [this](const QDateTime &current_time) {
		// shvInfo() << "current time:" << current_time;
		if (m_starterTime.time().minute() != current_time.time().minute()) {
			m_starterTime = current_time;

			// clear seconds, should be == 0 already
			auto tm = m_starterTime.time();
			if (tm.second() != 0) {
				tm.setHMS(tm.hour(), tm.minute(), 0);
				m_starterTime.setTime(tm);
			}

			auto line_time = m_starterTime.addSecs(-60);
			//shvInfo() << "UPDATE===================" << m_corridorTime;
			//ui->tableView->update();
			for (auto row = 0; row < rowCount(); ++row) {
				auto start = roleValue(row, Role::StartDateTime).toDateTime();
				auto diff = line_time.secsTo(start);
				if (diff >= 0 && diff <= (60 * 4)) {
					auto ix = index(row, 0);
					emit dataChanged(ix, ix);
				}
			}
		}
	});
}

void StartListModel::setResult(const Result &result)
{
	m_unconfirmedRecordChanges.clear();
	Super::setResult(result);
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
	// column names MUST in be lower case because of SQL
	switch (role) {
	case Role::RunId: return QStringLiteral("runs.id");
	case Role::CompetitorName: return QStringLiteral("competitorname");
	case Role::Registration: return QStringLiteral("competitors.registration");
	case Role::ClassName: return QStringLiteral("classes.name");
	case Role::StartTime: return QStringLiteral("runs.starttimems");
	case Role::SiId: return QStringLiteral("runs.siid");
	case Role::CorridorTime: return QStringLiteral("runs.corridortime");
	case Role::IsSelectedRow: throw std::runtime_error("IsSelectedRow role has not column");
	case Role::Corridor: throw std::runtime_error("Corridor role has not column");
	case Role::StartDateTime: throw std::runtime_error("StartDateTime role has not column");
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
	if (role == Role::StartDateTime) {
		auto start00_time = Application::instance()->currentStageStart();
		auto start_msec = roleValue(row, Role::StartTime).toInt();
		auto start_time = start00_time.addMSecs(start_msec);
		return start_time;
	}
	if (role == Role::Corridor) {
		auto start_time = roleValue(row, Role::StartDateTime).toDateTime();
		CorridorStage corridor = CEarly;
		auto sec_diff = m_starterTime.secsTo(start_time);
		if (sec_diff <= 0) corridor = CStarted;
		else if (sec_diff <= 60) corridor = C1;
		else if (sec_diff <= 2*60) corridor = C2;
		else if (sec_diff <= 3*60) corridor = C3;
		else corridor = CEarly;
		shvDebug() << "name:" << roleValue(row, Role::CompetitorName).toString()
				   << "sec diff:" << sec_diff
				   << "Corridor:" << static_cast<int>(corridor)
				   << "starter:" << m_starterTime.toString(Qt::ISODate)
					<< "start:" << start_time.toString(Qt::ISODate);
		return static_cast<int>(corridor);
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

void StartListModel::updateLocalRecord(int run_id, const QMap<StartListModel::Role, QVariant> &record)
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
					if (colname == roleToName(Role::CorridorTime).value_or("")) {
						auto corridor_time = val.toDateTime();
						if (!checkCorridorTime(row, corridor_time)) {
							emit corridorTimeCheckError();
							continue;
						}
					}
					chngmap[colname.value()] = v2;
				}
			}
		}
		if (!chngmap.isEmpty()) {
			shvDebug() << "setting unconfirmed value on run_id:" << run_id << "map:" << shv::coreqt::rpc::qVariantToPrettyString(chngmap, "  ");
			m_unconfirmedRecordChanges[run_id].insert(chngmap);
			shvDebug() << "new unconfirmed value on run_id:" << run_id << "map:" << shv::coreqt::rpc::qVariantToPrettyString(m_unconfirmedRecordChanges[run_id], "  ");
			emit localRecordUpdated(run_id, chngmap);
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

QVariantMap StartListModel::unconfirmedRecordChanges() const
{
	QVariantMap ret;
	for (const auto &[k, v] : m_unconfirmedRecordChanges.asKeyValueRange()) {
		if (!v.isEmpty()) {
			ret[QString::number(k)] = v;
		}
	}
	return ret;
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

void StartListModel::applyRemoteRecordChanges(int run_id, const QVariant &record)
{
	if (record.isNull() || !record.isValid()) {
		if (auto o_row = runIdToRow(run_id); o_row.has_value()) {
			auto row = o_row.value();
			beginRemoveRows({}, row, row);
			m_result.rows.removeAt(row);
			m_unconfirmedRecordChanges.remove(run_id);
			endRemoveRows();
		}
	}
	else {
		auto rec = record.toMap();
		if (auto o_row = runIdToRow(run_id); o_row.has_value()) {
			auto row = o_row.value();
			for (const auto &[key, val] : rec.asKeyValueRange()) {
				m_result.setValue(row, key, val);
				shvDebug() << "removing unconfirmed value on run_id:" << run_id << "key:" << key;
				m_unconfirmedRecordChanges[run_id].remove(key);
				shvDebug() << "new unconfirmed value on run_id:" << run_id << "map:" << shv::coreqt::rpc::qVariantToPrettyString(m_unconfirmedRecordChanges[run_id], "  ");
			}
			auto ix = createIndex(row, 0);
			emit dataChanged(ix, ix);
		}
		else {
			auto row = rowCount();
			beginInsertRows({}, row, row);
			for (const auto &[key, val] : rec.asKeyValueRange()) {
				m_result.setValue(row, key, val);
			}
			endInsertRows();
		}
	}
}

bool StartListModel::checkCorridorTime(int row, const QDateTime &corridor_time)
{
	if (!corridor_time.isValid()) {
		return true;
	}
	auto corridor = static_cast<StartListModel::CorridorStage>(roleValue(row, Role::Corridor).toInt());
	switch (corridor) {
	case CEarly: return false;
	case C3: return true;
	case C2: return true;
	case C1: return true;
	case CStarted: return true;
	}
	return true;
}

