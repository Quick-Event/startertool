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
	QVariant data(const QModelIndex &index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

	void setResult(const Result &result);
protected:
	Result m_result;
};

class StartListModel : public RpcSqlResultModel
{
	Q_OBJECT

	using Super = RpcSqlResultModel;
public:
	enum Role {RunId = Qt::UserRole, CompetitorName, Registration, ClassName, StartTime};

	explicit StartListModel(QObject *parent = nullptr);

	int columnCount(const QModelIndex &) const override { return 1; }
	QVariant data(const QModelIndex &index, int role) const override;
private:
	std::optional<int> roleToColumn(Role role) const;
	QVariant roleValue(int row, Role role) const;
	void onRunChanged(int run_id, const QVariant &record);
private:
	mutable QMap<QString, int> m_nameToIndex;
};

#endif // RPCSQLRESULTMODEL_H
