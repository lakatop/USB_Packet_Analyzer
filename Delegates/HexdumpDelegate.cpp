#include "HexdumpDelegate.h"

/// <summary>
/// Constructor of DataViewerDelegate.
/// </summary>
/// <param name="dataHighlight"><see cref="dataHighlight"/></param>
/// <param name="parent">Dialog <see cref="DataViewer"/></param>
HexdumpDelegate::HexdumpDelegate(bool dataHighlight, QObject *parent)
	: QStyledItemDelegate(parent)
{
	this->dataHighlight = dataHighlight;
	holder = DataHolder::GetDataHolder();
}

/// <summary>
/// Desctructor of DataViewerDelegate.
/// </summary>
HexdumpDelegate::~HexdumpDelegate()
{
}
/// <summary>
/// Determines size of cells in hexdump
/// </summary>
/// <param name="option">Not used in this scenario, we are returning fixed size</param>
/// <param name="index">Not used in this scenario, we are returning fixed size</param>
/// <returns>QSize of cells in hexdump</returns>
QSize HexdumpDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return QSize(30, 30);
}

/// <summary>
/// Paints hexdump data.
/// </summary>
/// <param name="painter">Given painter</param>
/// <param name="option">Describe the parameters used to draw an item</param>
/// <param name="index">Index to concrete data we want to draw</param>
void HexdumpDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QVariantList list;
	painter->save();
	QVariant data = index.data();
	if (data.canConvert<QVariantList>())
	{
		QStyledItemDelegate::paint(painter, option, index);
		list = data.toList();
		if (option.state & QStyle::State_Selected)
		{
			painter->fillRect(option.rect, option.palette.highlight());
		}
		painter->drawText(option.rect, Qt::AlignCenter | Qt::AlignVCenter, list[0].toString());
	}
	else
	{
		QStyledItemDelegate::paint(painter, option, index);
		if (option.state & QStyle::State_Selected)
		{
			painter->fillRect(option.rect, option.palette.highlight());
		}
	}
	uint penWidth = 3;
	QRect rect = option.rect;
	rect.setX(option.rect.x() + penWidth / 2);
	rect.setY(option.rect.y() + penWidth / 2);
	rect.setWidth(option.rect.width() - penWidth);
	rect.setHeight(option.rect.height() - penWidth);

	if (dataHighlight && list.size() == 2)
	{
		drawDataHighlight(painter, option, list);
	}

	painter->restore();
}

/// <summary>
/// Highlights hexdump data.
/// </summary>
/// <param name="painter">Given painter</param>
/// <param name="option">Describe the parameters used to draw an item</param>
/// <param name="list">QVariant list containing of HeaderDataType from which we determine color of highlighting</param>
void HexdumpDelegate::drawDataHighlight(QPainter* painter, const QStyleOptionViewItem& option, const QVariantList& list) const
{
	uint penWidth = 3;
	QRect rect = option.rect;
	rect.setX(option.rect.x() + penWidth / 2);
	rect.setY(option.rect.y() + penWidth / 2);
	rect.setWidth(option.rect.width() - penWidth);
	rect.setHeight(option.rect.height() - penWidth);

	switch (list[1].toInt())
	{
	case INTERR_TRANSFER:
	case ISOCHRO_TRANSFER:
	case BULK_TRANSFER:
	case CONTROL_TRANSFER_RESPONSE:
	case CONTROL_TRANSFER_HID_REPORT_DESC:
	case ADDITIONAL_HEADER_DATA:
	case HEADER_DATA:
	{
		drawOtherDataHighlight(painter, holder->DataColors[(HeaderDataType)list[1].toInt()], rect);
	}
	break;
	case CONTROL_TRANSFER_DEVICE_DESC:
	case CONTROL_TRANSFER_CONFIG_DESC:
	case CONTROL_TRANSFER_STRING_DESC:
	case CONTROL_TRANSFER_INTERF_DESC:
	case CONTROL_TRANSFER_ENDPOI_DESC:
	case CONTROL_TRANSFER_HID_DESC:
	case CONTROL_TRANSFER_SETUP:
	case CONTROL_TRANSFER_UNSPEC_DESC:
	{
		drawControlHeaderDataHiglight(painter, holder->DataColors[(HeaderDataType)list[1].toInt()], rect);
	}
	break;
	}
}

/// <summary>
/// Highlighting data other than Control Transfer type.
/// </summary>
/// <param name="painter">Given painter</param>
/// <param name="color">Color used for highliting</param>
/// <param name="rect">Rectangle which will be highlighted</param>
void HexdumpDelegate::drawOtherDataHighlight(QPainter* painter, DataTypeColor& color, QRect& rect) const
{
	uint penWidth = 3;
	painter->setPen(QPen(QColor::fromRgb(color.red, color.green, color.blue), penWidth));
	painter->drawRect(rect);
}

/// <summary>
/// Highlighting Control Transfer data type.
/// </summary>
/// <param name="painter">Given painter</param>
/// <param name="color">Color used for highliting</param>
/// <param name="rect">Rectangle which will be highlighted</param>
void HexdumpDelegate::drawControlHeaderDataHiglight(QPainter* painter, DataTypeColor& color, QRect& rect) const
{
	painter->fillRect(rect, QColor(color.red, color.green, color.blue, color.alpha));
	uint penWidth = 3;
	DataTypeColor controlTransferColor = holder->DataColors[CONTROL_TRANSFER];
	painter->setPen(QPen(QColor::fromRgb(controlTransferColor.red, controlTransferColor.green, controlTransferColor.blue), penWidth));
	painter->drawRect(rect);
}