#pragma once

enum class TEXT_ALIGNMENT
{
	/// <summary>
	/// The leading edge of the paragraph text is aligned to the layout box's leading edge.
	/// </summary>
	LEADING,

	/// <summary>
	/// The trailing edge of the paragraph text is aligned to the layout box's trailing edge.
	/// </summary>
	TRAILING,

	/// <summary>
	/// The center of the paragraph text is aligned to the center of the layout box.
	/// </summary>
	CENTER,

	/// <summary>
	/// Align text to the leading side, and also justify text to fill the lines.
	/// </summary>
	JUSTIFIED
};

enum class PARAGRAPH_ALIGNMENT
{
	/// <summary>
	/// The first line of paragraph is aligned to the flow's beginning edge of the layout box.
	/// </summary>
	NEAR_ALIGNMENT,

	/// <summary>
	/// The last line of paragraph is aligned to the flow's ending edge of the layout box.
	/// </summary>
	FAR_ALIGNMENT,

	/// <summary>
	/// The center of the paragraph is aligned to the center of the flow of the layout box.
	/// </summary>
	CENTER_ALIGNMENT
};

enum class WORD_WRAPPING
{
	/// <summary>
	/// Words are broken across lines to avoid text overflowing the layout box.
	/// </summary>
	WRAP,

	/// <summary>
	/// Words are kept within the same line even when it overflows the layout box.
	/// This option is often used with scrolling to reveal overflow text. 
	/// </summary>
	NO_WRAP
};