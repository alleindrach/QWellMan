#include <QComboBox>
#include <QTableWidget>
#include "QPointer"
#include "librarydelegate.h"


LibraryDelegate::LibraryDelegate(QWellTableModel * ref,LibraryModel * refModel,QString keyField, QObject *parent) :
    QStyledItemDelegate(parent),m_libmodel(ref),m_refModel(refModel),m_keyField(keyField)
{
    for(int i=0;i<refModel->localProperties().size();i++){
        if(refModel->localProperties()[i]==keyField){
            m_refKeyField=refModel->refColumns()[i];
            break;
        }
    }
}


QWidget *LibraryDelegate::createEditor(QWidget *parent,
                                       const QStyleOptionViewItem & option,
                                       const QModelIndex & index) const
{
    QTableView *editor = new QTableView(parent);
    editor->setModel(m_libmodel);
    connect(editor,SIGNAL(doubleClicked(const QModelIndex)),this,SLOT(commitAndCloseEditor(const QModelIndex &)));
    editor->setSelectionBehavior(QAbstractItemView::SelectRows);
    editor->setSelectionMode( QAbstractItemView::SingleSelection);
    editor->installEventFilter(const_cast<LibraryDelegate*>(this));
    return editor;
}


void LibraryDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const
{


    QTableView *delegate = static_cast<QTableView*>(editor);
    QWellTableModel *  model= ( QWellTableModel *)index.model();
    //    delegate->setModel(m_libmodel);
    QVariant value=index.data();
    if(value.isNull())
        return;
    if(value.type()==QMetaType::QString)
    {
        if(value.toString().isEmpty())
            return;
    }
    int refKeyCol=m_libmodel->sectionOfField(m_refKeyField);
    QModelIndexList searchResult=m_libmodel->match(m_libmodel->index(0,refKeyCol),Qt::DisplayRole,value,1,Qt::MatchExactly|Qt::MatchRecursive);
    QModelIndex loc;
    loc=searchResult.first();
    if(loc.isValid())
    {
        delegate->selectRow(loc.row());
        delegate->setProperty("selectedRow",loc.row());
        qDebug()<<"delegate <<"<< (void*)delegate<<" assigned";
    };

}


void LibraryDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const
{
    QTableView *delegate  = static_cast<QTableView*>(editor);

    Nut::Row<Nut::Table> row=m_libmodel->at( delegate->currentIndex().row());
    QWellTableModel * dataModel=static_cast<QWellTableModel*>(model);
    QVariant  oldValueAsVariant=delegate->property("selectedRow");
    int  oldValue=oldValueAsVariant.isValid()?oldValueAsVariant.toInt():-1;
    if(oldValue!=delegate->currentIndex().row()){
        for(int i=0;i<m_refModel->localProperties().size();i++)
        {
            QString localPropertyName=m_refModel->localProperties()[i];
            QString refPropertyName=m_refModel->refColumns()[i];
            //录入表的字段位置
            int loc=dataModel->sectionOfField(localPropertyName);
            int lor=dataModel->mode()==Qt::Horizontal?index.row():index.column();
            QVariant newRefValue=m_libmodel->at(delegate->currentIndex().row())->property(refPropertyName.toUtf8().data());
            model->setData(dataModel->mode()==Qt::Horizontal? model->index(lor,loc):model->index(loc,lor),newRefValue,Qt::DisplayRole);
        }
    }



}
void LibraryDelegate::updateEditorGeometry(QWidget *editor,
                                           const QStyleOptionViewItem &option, const QModelIndex & index) const
{

    QWellTableModel *  model= ( QWellTableModel *)index.model();
    if(model->mode()==Qt::Horizontal){
        QPoint topleft(option.rect.left()-100, option.rect.top());
        editor->setGeometry(QRect(topleft,QSize(400,200)));
    }else
    {
        QPoint topleft(option.rect.left(), option.rect.top()-10);
        editor->setGeometry(QRect(topleft,QSize(400,200)));
    }
}

void LibraryDelegate::commitAndCloseEditor(const QModelIndex &index)
{
    qDebug()<<"on_treeView_doubleClicked";
    QTableView *editor = qobject_cast<QTableView *>(sender());
    emit commitData(editor);
    emit closeEditor(editor);
}
