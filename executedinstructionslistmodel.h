#ifndef EXECUTEDINSTRUCTIONSLISTMODEL_H
#define EXECUTEDINSTRUCTIONSLISTMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QList>
#include <QMutex>

class ExecutedInstructionsListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum ExecutedInstructionsItemDataRole {
        CPUStateRole = Qt::UserRole + 1,
    };
    explicit ExecutedInstructionsListModel(QObject *parent = nullptr);
    ~ExecutedInstructionsListModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    void addInstruction(QList<QVariant> executed_instruction);

signals:

public slots:

private:
    QList<QList<QVariant>> instructionsList;
    QMutex *mu;
};

#endif // EXECUTEDINSTRUCTIONSLISTMODEL_H
