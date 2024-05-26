#ifndef STARTLISTMODEL_H
#define STARTLISTMODEL_H

#include "rpcsqlresultmodel.h"

#include <QDateTime>

class StartListModel : public RpcSqlResultModel
{
	Q_OBJECT

	using Super = RpcSqlResultModel;
public:
	enum Role {RunId = Qt::UserRole, CompetitorName, Registration, ClassName, StartTime, CorridorTime, SiId};

	explicit StartListModel(QObject *parent = nullptr);

	int columnCount(const QModelIndex &) const override { return 1; }
	QVariant data(const QModelIndex &index, int role) const override;

	QVariant roleValue(int row, Role role) const;
	void setRoleValue(int row, Role role, const QVariant &val);
	std::optional<int> runIdToRow(int run_id) const;
	QVariant recordValue(int run_id, Role role) const;

	void setRecord(int run_id, const QMap<Role, QVariant> &record);
	Q_SIGNAL void recordChanged(int run_id, const QVariant &record);
private:
	QVariant retypeValue(const QVariant &val, Role role) const;
	std::optional<int> roleToColumn(Role role) const;
	std::optional<QString> roleToName(Role role) const;
	void onRunChanged(int run_id, const QVariant &record);
private:
	const QMap<Role, QMetaType::Type> m_roleTypes;
	mutable QMap<QString, int> m_nameToIndex;
	QDateTime m_corridorTime;
};

#endif // STARTLISTMODEL_H
