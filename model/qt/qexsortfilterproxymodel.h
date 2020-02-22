/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QEXSORTFILTERPROXYMODEL_H
#define QEXSORTFILTERPROXYMODEL_H

#include <QtCore/qabstractproxymodel.h>
#include <QtCore/qregexp.h>
#include "private/qabstractproxymodel_p.h"
#if QT_CONFIG(regularexpression)
# include <QtCore/qregularexpression.h>
#endif

QT_REQUIRE_CONFIG(sortfilterproxymodel);

QT_BEGIN_NAMESPACE

QT_FORWARD_DECLARE_CLASS(QExSortFilterProxyModelPrivate);
QT_FORWARD_DECLARE_CLASS(QExSortFilterProxyModelLessThan);
QT_FORWARD_DECLARE_CLASS(QExSortFilterProxyModelGreaterThan);

typedef QVector<QPair<QModelIndex, QPersistentModelIndex> > QModelIndexPairList;


//this struct is used to store what are the rows that are removed
//between a call to rowsAboutToBeRemoved and rowsRemoved
//it avoids readding rows to the mapping that are currently being removed
struct QRowsRemoval
{
    QRowsRemoval(const QModelIndex &parent_source, int start, int end) : parent_source(parent_source), start(start), end(end)
    {
    }

    QRowsRemoval() : start(-1), end(-1)
    {
    }

    bool contains(QModelIndex parent, int row) const
    {
        do {
            if (parent == parent_source)
                return row >= start && row <= end;
            row = parent.row();
            parent = parent.parent();
        } while (row >= 0);
        return false;
    }
private:
    QModelIndex parent_source;
    int start;
    int end;
};

class RegularExpressionData {

private:
    enum class ExpressionType {
        RegExp,
#if QT_CONFIG(regularexpression)
        RegularExpression
#endif
    };

public:
    RegularExpressionData() :
        m_type(ExpressionType::RegExp)
    {}

#if QT_CONFIG(regularexpression)
    QRegularExpression regularExpression() const
    {
        if (m_type == ExpressionType::RegularExpression)
            return  m_regularExpression;
        return QRegularExpression();
    }

    void setRegularExpression(const QRegularExpression &rx)
    {
        m_type = ExpressionType::RegularExpression;
        m_regularExpression = rx;
        m_regExp = QRegExp();
    }
#endif

    QRegExp regExp() const
    {
        if (m_type == ExpressionType::RegExp)
            return m_regExp;
        return QRegExp();
    }

    void setRegExp(const QRegExp &rx)
    {
        m_type = ExpressionType::RegExp;
        m_regExp = rx;
#if QT_CONFIG(regularexpression)
        m_regularExpression = QRegularExpression();
#endif

    }

    bool isEmpty() const
    {
        bool result = true;
        switch (m_type) {
        case ExpressionType::RegExp:
            result = m_regExp.isEmpty();
            break;
#if QT_CONFIG(regularexpression)
        case ExpressionType::RegularExpression:
            result = m_regularExpression.pattern().isEmpty();
            break;
#endif
        }
        return result;
    }

    Qt::CaseSensitivity caseSensitivity() const
    {
        Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive;
        switch (m_type) {
        case ExpressionType::RegExp:
            sensitivity = m_regExp.caseSensitivity();
            break;
#if QT_CONFIG(regularexpression)
        case ExpressionType::RegularExpression:
        {
            QRegularExpression::PatternOptions options = m_regularExpression.patternOptions();
            if (!(options & QRegularExpression::CaseInsensitiveOption))
                sensitivity = Qt::CaseSensitive;
        }
            break;
#endif
        }
        return sensitivity;
    }

    void setCaseSensitivity(Qt::CaseSensitivity cs)
    {
        switch (m_type) {
        case ExpressionType::RegExp:
            m_regExp.setCaseSensitivity(cs);
            break;
#if QT_CONFIG(regularexpression)
        case ExpressionType::RegularExpression:
        {
            QRegularExpression::PatternOptions options = m_regularExpression.patternOptions();
            options.setFlag(QRegularExpression::CaseInsensitiveOption, cs == Qt::CaseSensitive);
            m_regularExpression.setPatternOptions(options);
        }
            break;
#endif
        }
    }

    bool hasMatch(const QString &str) const
    {
        bool result = false;
        switch (m_type) {
        case ExpressionType::RegExp:
            result = str.contains(m_regExp);
            break;
#if QT_CONFIG(regularexpression)
        case ExpressionType::RegularExpression:
            result = str.contains(m_regularExpression);
            break;
#endif
        }
        return result;
    }

private:
    ExpressionType m_type;
    QRegExp m_regExp;
#if QT_CONFIG(regularexpression)
    QRegularExpression m_regularExpression;
#endif
};


class Q_CORE_EXPORT QExSortFilterProxyModel : public QAbstractProxyModel
{
    friend class QExSortFilterProxyModelLessThan;
    friend class QExSortFilterProxyModelGreaterThan;

    Q_OBJECT
    Q_PROPERTY(QRegExp filterRegExp READ filterRegExp WRITE setFilterRegExp)
#if QT_CONFIG(regularexpression)
    Q_PROPERTY(QRegularExpression filterRegularExpression READ filterRegularExpression WRITE setFilterRegularExpression)
#endif
    Q_PROPERTY(int filterKeyColumn READ filterKeyColumn WRITE setFilterKeyColumn)
    Q_PROPERTY(bool dynamicSortFilter READ dynamicSortFilter WRITE setDynamicSortFilter)
    Q_PROPERTY(Qt::CaseSensitivity filterCaseSensitivity READ filterCaseSensitivity WRITE setFilterCaseSensitivity)
    Q_PROPERTY(Qt::CaseSensitivity sortCaseSensitivity READ sortCaseSensitivity WRITE setSortCaseSensitivity)
    Q_PROPERTY(bool isSortLocaleAware READ isSortLocaleAware WRITE setSortLocaleAware)
    Q_PROPERTY(int sortRole READ sortRole WRITE setSortRole)
    Q_PROPERTY(int filterRole READ filterRole WRITE setFilterRole)
    Q_PROPERTY(bool recursiveFilteringEnabled READ isRecursiveFilteringEnabled WRITE setRecursiveFilteringEnabled)

public:
    explicit QExSortFilterProxyModel(QObject *parent = nullptr);
    ~QExSortFilterProxyModel();

    void setSourceModel(QAbstractItemModel *sourceModel) override;

    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;

    QItemSelection mapSelectionToSource(const QItemSelection &proxySelection) const override;
    QItemSelection mapSelectionFromSource(const QItemSelection &sourceSelection) const override;

    QRegExp filterRegExp() const;

#if QT_CONFIG(regularexpression)
    QRegularExpression filterRegularExpression() const;
#endif

    int filterKeyColumn() const;
    void setFilterKeyColumn(int column);

    Qt::CaseSensitivity filterCaseSensitivity() const;
    void setFilterCaseSensitivity(Qt::CaseSensitivity cs);

    Qt::CaseSensitivity sortCaseSensitivity() const;
    void setSortCaseSensitivity(Qt::CaseSensitivity cs);

    bool isSortLocaleAware() const;
    void setSortLocaleAware(bool on);

    int sortColumn() const;
    Qt::SortOrder sortOrder() const;

    bool dynamicSortFilter() const;
    void setDynamicSortFilter(bool enable);

    int sortRole() const;
    void setSortRole(int role);

    int filterRole() const;
    void setFilterRole(int role);

    bool isRecursiveFilteringEnabled() const;
    void setRecursiveFilteringEnabled(bool recursive);

public Q_SLOTS:
    void setFilterRegExp(const QString &pattern);
    void setFilterRegExp(const QRegExp &regExp);
#if QT_CONFIG(regularexpression)
    void setFilterRegularExpression(const QString &pattern);
    void setFilterRegularExpression(const QRegularExpression &regularExpression);
#endif
    void setFilterWildcard(const QString &pattern);
    void setFilterFixedString(const QString &pattern);
#if QT_DEPRECATED_SINCE(5, 11)
    QT_DEPRECATED_X("Use QSortFilterProxyModel::invalidate") void clear();
#endif
    void invalidate();

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
    virtual bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const;
    virtual bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;

#if QT_DEPRECATED_SINCE(5, 11)
    QT_DEPRECATED_X("Use QSortFilterProxyModel::invalidateFilter") void filterChanged();
#endif
    void invalidateFilter();

public:
    using QObject::parent;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    QModelIndex sibling(int row, int column, const QModelIndex &idx) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    bool setHeaderData(int section, Qt::Orientation orientation,
            const QVariant &value, int role = Qt::EditRole) override;

    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action,
                      int row, int column, const QModelIndex &parent) override;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool insertColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeColumns(int column, int count, const QModelIndex &parent = QModelIndex()) override;

    void fetchMore(const QModelIndex &parent) override;
    bool canFetchMore(const QModelIndex &parent) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    QModelIndex buddy(const QModelIndex &index) const override;
    QModelIndexList match(const QModelIndex &start, int role,
                          const QVariant &value, int hits = 1,
                          Qt::MatchFlags flags =
                          Qt::MatchFlags(Qt::MatchStartsWith|Qt::MatchWrap)) const override;
    QSize span(const QModelIndex &index) const override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    QStringList mimeTypes() const override;
    Qt::DropActions supportedDropActions() const override;
private:
    Q_DECLARE_PRIVATE(QExSortFilterProxyModel)
    Q_DISABLE_COPY(QExSortFilterProxyModel)

    Q_PRIVATE_SLOT(d_func(), void _q_sourceDataChanged(const QModelIndex &source_top_left, const QModelIndex &source_bottom_right, const QVector<int> &roles))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceHeaderDataChanged(Qt::Orientation orientation, int start, int end))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceAboutToBeReset())
    Q_PRIVATE_SLOT(d_func(), void _q_sourceReset())
    Q_PRIVATE_SLOT(d_func(), void _q_sourceLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceLayoutChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceRowsAboutToBeInserted(const QModelIndex &source_parent, int start, int end))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceRowsInserted(const QModelIndex &source_parent, int start, int end))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceRowsAboutToBeRemoved(const QModelIndex &source_parent, int start, int end))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceRowsRemoved(const QModelIndex &source_parent, int start, int end))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceRowsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceRowsMoved(QModelIndex,int,int,QModelIndex,int))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceColumnsAboutToBeInserted(const QModelIndex &source_parent, int start, int end))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceColumnsInserted(const QModelIndex &source_parent, int start, int end))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceColumnsAboutToBeRemoved(const QModelIndex &source_parent, int start, int end))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceColumnsRemoved(const QModelIndex &source_parent, int start, int end))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceColumnsAboutToBeMoved(QModelIndex,int,int,QModelIndex,int))
    Q_PRIVATE_SLOT(d_func(), void _q_sourceColumnsMoved(QModelIndex,int,int,QModelIndex,int))
    Q_PRIVATE_SLOT(d_func(), void _q_clearMapping())
};


class QExSortFilterProxyModelPrivate : public QAbstractProxyModelPrivate
{
    Q_DECLARE_PUBLIC(QExSortFilterProxyModel)

public:
    struct Mapping {
        QVector<int> source_rows;
        QVector<int> source_columns;
        QVector<int> proxy_rows;
        QVector<int> proxy_columns;
        QVector<QModelIndex> mapped_children;
        QHash<QModelIndex, Mapping *>::const_iterator map_iter;
    };

    mutable QHash<QModelIndex, Mapping*> source_index_mapping;

    int source_sort_column;
    int proxy_sort_column;
    Qt::SortOrder sort_order;
    Qt::CaseSensitivity sort_casesensitivity;
    int sort_role;
    bool sort_localeaware;

    int filter_column;
    int filter_role;
    RegularExpressionData filter_data;
    QModelIndex last_top_source;

    bool filter_recursive;
    bool complete_insert;
    bool dynamic_sortfilter;
    QRowsRemoval itemsBeingRemoved;

    QModelIndexPairList saved_persistent_indexes;
    QList<QPersistentModelIndex> saved_layoutChange_parents;

    QHash<QModelIndex, Mapping *>::const_iterator create_mapping(
        const QModelIndex &source_parent) const;
    QModelIndex proxy_to_source(const QModelIndex &proxyIndex) const;
    QModelIndex source_to_proxy(const QModelIndex &sourceIndex) const;
    bool can_create_mapping(const QModelIndex &source_parent) const;

    void remove_from_mapping(const QModelIndex &source_parent);

    inline QHash<QModelIndex, Mapping *>::const_iterator index_to_iterator(
        const QModelIndex &proxy_index) const
    {
        Q_ASSERT(proxy_index.isValid());
        Q_ASSERT(proxy_index.model() == q_func());
        const void *p = proxy_index.internalPointer();
        Q_ASSERT(p);
        QHash<QModelIndex, Mapping *>::const_iterator it =
            static_cast<const Mapping*>(p)->map_iter;
        Q_ASSERT(it != source_index_mapping.constEnd());
        Q_ASSERT(it.value());
        return it;
    }

    inline QModelIndex create_index(int row, int column,
                                    QHash<QModelIndex, Mapping*>::const_iterator it) const
    {
        return q_func()->createIndex(row, column, *it);
    }

    void _q_sourceDataChanged(const QModelIndex &source_top_left,
                              const QModelIndex &source_bottom_right,
                              const QVector<int> &roles);
    void _q_sourceHeaderDataChanged(Qt::Orientation orientation, int start, int end);

    void _q_sourceAboutToBeReset();
    void _q_sourceReset();

    void _q_sourceLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint);
    void _q_sourceLayoutChanged(const QList<QPersistentModelIndex> &sourceParents, QAbstractItemModel::LayoutChangeHint hint);

    void _q_sourceRowsAboutToBeInserted(const QModelIndex &source_parent,
                                        int start, int end);
    void _q_sourceRowsInserted(const QModelIndex &source_parent,
                               int start, int end);
    void _q_sourceRowsAboutToBeRemoved(const QModelIndex &source_parent,
                                       int start, int end);
    void _q_sourceRowsRemoved(const QModelIndex &source_parent,
                              int start, int end);
    void _q_sourceRowsAboutToBeMoved(const QModelIndex &sourceParent,
                                     int sourceStart, int sourceEnd,
                                     const QModelIndex &destParent, int dest);
    void _q_sourceRowsMoved(const QModelIndex &sourceParent,
                            int sourceStart, int sourceEnd,
                            const QModelIndex &destParent, int dest);
    void _q_sourceColumnsAboutToBeInserted(const QModelIndex &source_parent,
                                           int start, int end);
    void _q_sourceColumnsInserted(const QModelIndex &source_parent,
                                  int start, int end);
    void _q_sourceColumnsAboutToBeRemoved(const QModelIndex &source_parent,
                                          int start, int end);
    void _q_sourceColumnsRemoved(const QModelIndex &source_parent,
                                 int start, int end);
    void _q_sourceColumnsAboutToBeMoved(const QModelIndex &sourceParent,
                                        int sourceStart, int sourceEnd,
                                        const QModelIndex &destParent, int dest);
    void _q_sourceColumnsMoved(const QModelIndex &sourceParent,
                               int sourceStart, int sourceEnd,
                               const QModelIndex &destParent, int dest);

    void _q_clearMapping();

    void sort();
    bool update_source_sort_column();
    int find_source_sort_column() const;
    void sort_source_rows(QVector<int> &source_rows,
                          const QModelIndex &source_parent) const;
    QVector<QPair<int, QVector<int > > > proxy_intervals_for_source_items_to_add(
        const QVector<int> &proxy_to_source, const QVector<int> &source_items,
        const QModelIndex &source_parent, Qt::Orientation orient) const;
    QVector<QPair<int, int > > proxy_intervals_for_source_items(
        const QVector<int> &source_to_proxy, const QVector<int> &source_items) const;
    void insert_source_items(
        QVector<int> &source_to_proxy, QVector<int> &proxy_to_source,
        const QVector<int> &source_items, const QModelIndex &source_parent,
        Qt::Orientation orient, bool emit_signal = true);
    void remove_source_items(
        QVector<int> &source_to_proxy, QVector<int> &proxy_to_source,
        const QVector<int> &source_items, const QModelIndex &source_parent,
        Qt::Orientation orient, bool emit_signal = true);
    void remove_proxy_interval(
        QVector<int> &source_to_proxy, QVector<int> &proxy_to_source,
        int proxy_start, int proxy_end, const QModelIndex &proxy_parent,
        Qt::Orientation orient, bool emit_signal = true);
    void build_source_to_proxy_mapping(
        const QVector<int> &proxy_to_source, QVector<int> &source_to_proxy) const;
    void source_items_inserted(const QModelIndex &source_parent,
                               int start, int end, Qt::Orientation orient);
    void source_items_about_to_be_removed(const QModelIndex &source_parent,
                                          int start, int end, Qt::Orientation orient);
    void source_items_removed(const QModelIndex &source_parent,
                              int start, int end, Qt::Orientation orient);
    void proxy_item_range(
        const QVector<int> &source_to_proxy, const QVector<int> &source_items,
        int &proxy_low, int &proxy_high) const;

    QModelIndexPairList store_persistent_indexes() const;
    void update_persistent_indexes(const QModelIndexPairList &source_indexes);

    void filter_about_to_be_changed(const QModelIndex &source_parent = QModelIndex());
    void filter_changed(const QModelIndex &source_parent = QModelIndex());
    QSet<int> handle_filter_changed(
        QVector<int> &source_to_proxy, QVector<int> &proxy_to_source,
        const QModelIndex &source_parent, Qt::Orientation orient);

    void updateChildrenMapping(const QModelIndex &source_parent, Mapping *parent_mapping,
                               Qt::Orientation orient, int start, int end, int delta_item_count, bool remove);

    void _q_sourceModelDestroyed() override;

    bool needsReorder(const QVector<int> &source_rows, const QModelIndex &source_parent) const;

    bool filterAcceptsRowInternal(int source_row, const QModelIndex &source_parent) const;
    bool filterRecursiveAcceptsRow(int source_row, const QModelIndex &source_parent) const;
};

QT_END_NAMESPACE

#endif // QSORTFILTERPROXYMODEL_H
