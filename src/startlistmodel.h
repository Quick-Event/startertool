#ifndef STARTLISTMODEL_H
#define STARTLISTMODEL_H

#include "rpcsqlresultmodel.h"

#include <QDateTime>

class StartListModel : public RpcSqlResultModel
{
	Q_OBJECT

	using Super = RpcSqlResultModel;
public:
	enum Role {RunId = Qt::UserRole, CompetitorName, Registration, ClassName, StartTime, CorridorTime, SiId, StartDateTime, IsSelectedRow, Corridor};
	enum CorridorStage {CEarly, C3, C2, C1, CStarted};

	explicit StartListModel(QObject *parent = nullptr);

	virtual void setResult(const Result &result) override;

	int columnCount(const QModelIndex &) const override { return 1; }
	QVariant data(const QModelIndex &index, int role) const override;

	QVariant roleValue(int row, Role role) const;
	void setRoleValue(int row, Role role, const QVariant &val);
	std::optional<int> runIdToRow(int run_id) const;
	QVariant recordValue(int run_id, Role role) const;

	void updateLocalRecord(int run_id, const QMap<Role, QVariant> &record);
	Q_SIGNAL void localRecordUpdated(int run_id, const QVariant &record);

	void applyRemoteRecordChanges(int run_id, const QVariant &record);

	Q_SIGNAL void corridorTimeCheckError();

	void setSelectedRow(std::optional<int> row);
	QList<int> fullTextSearch(const QString &txt) const;

	QVariantMap unconfirmedRecordChanges() const;
private:
	QVariant retypeValue(const QVariant &val, Role role) const;
	std::optional<int> roleToColumn(Role role) const;
	std::optional<QString> roleToName(Role role) const;
	bool checkCorridorTime(int row, const QDateTime &corridor_time);
private:
	const QMap<Role, QMetaType::Type> m_roleTypes;
	mutable QMap<QString, int> m_nameToIndex;
	QDateTime m_starterTime;
	std::optional<int> m_selectedRow;
	QMap<int, QVariantMap> m_unconfirmedRecordChanges;
};

#endif // STARTLISTMODEL_H
