#include "executedinstructionslistmodel.h"

ExecutedInstructionsListModel::ExecutedInstructionsListModel(QObject *parent) : QAbstractListModel(parent)
{
    mu = new QMutex();
}

ExecutedInstructionsListModel::~ExecutedInstructionsListModel() {
    delete mu;
}

int ExecutedInstructionsListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    mu->lock();
    int length = instructionsList.length();
    mu->unlock();
    return length;
}

QVariant ExecutedInstructionsListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return QVariant();
    mu->lock();
    QList<QVariant> inst = instructionsList.at(index.row());
    mu->unlock();
    if (role == Qt::DisplayRole) {
        return inst.at(0);
    }
    if (role == CPUStateRole) {
        return QVariant(inst);
    }
    return QVariant();
}

void ExecutedInstructionsListModel::addInstruction(QList<QVariant> executed_instruction) {
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    mu->lock();
    instructionsList.append(executed_instruction);
    mu->unlock();
    endInsertRows();
}

