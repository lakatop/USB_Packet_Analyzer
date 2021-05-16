#ifndef DATAVIEWERDELEGATE_HPP
#define DATAVIEWERDELEGATE_HPP

#include <QStyledItemDelegate>

#include "../DefinedStructs/ConstDataHolder.hpp"

#include <qpainter.h>

/// <summary>
/// Delegate class for hexdump.
/// </summary>
class HexdumpDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:
	HexdumpDelegate(const bool dataHighlight, QObject *parent);
	~HexdumpDelegate();
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
private:
	void drawDataHighlight(QPainter* painter, const QStyleOptionViewItem& option, const QVariantList& list) const;
	void drawOtherDataHighlight(QPainter* painter, const DataTypeColor& color, const QRect& rect) const;
	void drawControlHeaderDataHiglight(QPainter* painter, const DataTypeColor& color, const QRect& rect) const;

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