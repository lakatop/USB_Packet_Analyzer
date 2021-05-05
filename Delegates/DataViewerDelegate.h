#ifndef DATAVIEWERDELEGATE_HPP
#define DATAVIEWERDELEGATE_HPP

#include <QStyledItemDelegate>

#include "../DefinedStructs/ConstDataHolder.hpp"

#include <qpainter.h>

/// <summary>
/// Delegate class for hexdump.
/// </summary>
class DataViewerDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	DataViewerDelegate(bool dataHighlight, QObject *parent);
	~DataViewerDelegate();
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
private:
	void drawDataHighlight(QPainter* painter, const QStyleOptionViewItem& option, const QVariantList& list) const;
	void drawOtherDataHighlight(QPainter* painter, DataTypeColor& color, QRect& rect) const;
	void drawControlHeaderDataHiglight(QPainter* painter, DataTypeColor& color, QRect& rect) const;

	/// <summary>
	/// Whether delegate should color highlight data
	/// </summary>
	bool dataHighlight;
	/// <summary>
	/// Pointer to DataHolder instance.
	/// </summary>
	DataHolder* holder;
};

#endif // !DATAVIEWERDELEGATE_HPP