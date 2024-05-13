#ifndef RPCSQLRESULTMODEL_H
#define RPCSQLRESULTMODEL_H

#include <QAbstractTableModel>
#include <shv/coreqt/data/rpcsqlresult.h>

class RpcSqlResultModel : public QAbstractTableModel
{
	Q_OBJECT

	using Super = QAbstractTableModel;
public:
	using Result = shv::coreqt::data::RpcSqlResult;
	using Field = shv::coreqt::data::RpcSqlField;

	explicit RpcSqlResultModel(QObject *parent = nullptr);

	int rowCount(const QModelIndex & = {}) const override;
	int columnCount(const QModelIndex &parent = {}) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	QVariant data(const QModelIndex &index, int role) const override;

	void setResult(const Result &result);
protected:
	Result m_result;
};

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
	std::optional<int> runIdToRow(int run_id) const;
	QVariant recordValue(int run_id, Role role) const;

	void setRecord(int run_id, const QMap<Role, QVariant> &record);
	Q_SIGNAL void recordChanged(int run_id, const QVariant &record);
private:
	std::optional<int> roleToColumn(Role role) const;
	std::optional<QString> roleToName(Role role) const;
	void onRunChanged(int run_id, const QVariant &record);
private:
	mutable QMap<QString, int> m_nameToIndex;
};

#endif // RPCSQLRESULTMODEL_H
