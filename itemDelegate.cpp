#include "itemDelegate.h"

ItemDelegate::ItemDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{

}

ItemDelegate::~ItemDelegate()
{

}

void ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const{
    if(index.isValid()){
        painter->save();
        QVariant var = index.data(Qt::UserRole+1);
        itemData mData = var.value<itemData>();

        //用来在视图中画一个item
        QStyleOptionViewItem viewOption(option);

        QRectF rect;
        rect.setX(option.rect.x());
        rect.setY(option.rect.y());
        rect.setWidth(option.rect.width()-1);
        //rect.setHeight(option.rect.height()-1);
        rect.setHeight(60);

        //QPainterPath画圆角矩形
        const qreal radius = 7;
        QPainterPath path;
        path.moveTo(rect.topRight() - QPointF(radius, 0));
        path.lineTo(rect.topLeft() + QPointF(radius, 0));
        path.quadTo(rect.topLeft(), rect.topLeft() + QPointF(0, radius));
        path.lineTo(rect.bottomLeft() + QPointF(0, -radius));
        path.quadTo(rect.bottomLeft(), rect.bottomLeft() + QPointF(radius, 0));
        path.lineTo(rect.bottomRight() - QPointF(radius, 0));
        path.quadTo(rect.bottomRight(), rect.bottomRight() + QPointF(0, -radius));
        path.lineTo(rect.topRight() + QPointF(0, radius));
        path.quadTo(rect.topRight(), rect.topRight() + QPointF(-radius, -0));

        //设置item被选中时的状态
//        if(option.state.testFlag(QStyle::State_Selected)){
//            painter->setPen(QPen(Qt::blue));
//            painter->setBrush(QColor(229, 241, 255));
//            painter->drawPath(path);
//        }
//        else{
//            painter->setPen(QPen(Qt::gray));
//            painter->setBrush(Qt::NoBrush);
//            painter->drawPath(path);
//        }

        painter->setPen(QPen(Qt::gray));
        painter->setBrush(Qt::NoBrush);
        painter->drawPath(path);

        //绘制item区域
        QRectF leftText = QRect(rect.left()+10, rect.top()+15, 130, 40);
        QRectF rightText = QRect(leftText.right()+30, rect.top()+15, 155, 40);

        painter->setPen(QPen(Qt::black));
        painter->setFont(QFont("Microsoft Yahei", 15));

        painter->drawText(leftText, mData.leftLabel);
        painter->drawText(rightText, mData.rightData);

        painter->restore();
    }
}

//设置item大小为220，50
QSize ItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    return QSize(220, 50);
}
