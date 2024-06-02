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

	virtual void setResult(const Result &result);
protected:
	Result m_result;
};

#endif // RPCSQLRESULTMODEL_H
