#ifndef UTILS_H
#define UTILS_H

class Utils {
public:
	/**
	 * Calculates the padding required to align a base address to a given alignment.
	 *
	 * @param baseAddress The base address to align.
	 * @param alignment The alignment to use.
	 * @return The padding required to align the base address to the given alignment.
	 */
	static const std::size_t CalculatePadding(const std::size_t baseAddress, const std::size_t alignment)
	{
		const std::size_t multiplier = (baseAddress / alignment) + 1;
		const std::size_t alignedAddress = multiplier * alignment;
		const std::size_t padding = alignedAddress - baseAddress;
		return padding;
	}

	/// Calculates the padding required to align a base address with a given alignment, taking into account the size of a header.
	///
	/// @param baseAddress The base address to align.
	/// @param alignment The alignment to use.
	/// @param headerSize The size of the header.
	/// @return The padding required to align the base address with the given alignment, accounting for the header size.
	static const std::size_t CalculatePaddingWithHeader(const std::size_t baseAddress, const std::size_t alignment, const std::size_t headerSize)
	{
		std::size_t padding = CalculatePadding(baseAddress, alignment);
		std::size_t neededSpace = headerSize;

		if (padding < neededSpace)
		{
			// Header does not fit - Calculate next aligned address that header fits
			neededSpace -= padding;

			// How many alignments I need to fit the header
			if (neededSpace % alignment > 0)
			{
				padding += alignment * (1 + (neededSpace / alignment));
			}
			else
			{
				padding += alignment * (neededSpace / alignment);
			}
		}

		return padding;
	}
};

#endif /* UTILS_H */