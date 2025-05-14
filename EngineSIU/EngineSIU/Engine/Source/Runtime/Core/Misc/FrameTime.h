
#pragma once
#include <type_traits>

#include "HAL/PlatformType.h"
#include "Math/MathUtility.h"
#include "Serialization/Archive.h"

/**
 * Structure representing a time by a context-free frame number, plus a sub frame value in the range [0:1)
 * Conversion to and from time in seconds is achieved in combination with FFrameRate.
 * Only the frame number part of this representation can be negative, sub frames are always a positive value between the frame number and its next logical frame
 */
struct FFrameTime
{
    inline static const float MaxSubframe = 0.99999994f;

	/**
	 * Default constructor initializing to zero
	 */
	FFrameTime();

	/**
	 * Implicit construction from a single integer, while disallowing implicit conversion from any other numeric type
	 */
	template<typename T, typename = std::enable_if_t<std::is_same_v<T, int32>>>
	FFrameTime(T /* int32 */ InFrameNumber);
 
	/**
	 * Implicit construction from a type-safe frame number
	 */
	FFrameTime(int32 InFrameNumber);

	/**
	 * Construction from a frame number and a sub frame
	 */
	FFrameTime(int32 InFrameNumber, float InSubFrame);

	/**
	 * Assignment from a type-safe frame number
	 */
	FFrameTime& operator=(int32 InFrameNumber);

	/**
	 * Serializes the given FrameTime from or into the specified archive
	 */
	bool Serialize(FArchive& Ar);

public:

	/**
	 * Access this time's frame number
	 */
	FORCEINLINE int32 GetFrame() const
	{
		return FrameNumber;
	}

	/**
	 * Access this time's sub frame
	 */
	FORCEINLINE float GetSubFrame() const
	{
		return SubFrame;
	}

	/**
	 * Return the first frame number less than or equal to this frame time
	 */
	int32 FloorToFrame() const;

	/**
	 * Return the next frame number greater than or equal to this frame time
	 */
	int32 CeilToFrame() const;

	/**
	 * Round to the nearest frame number
	 */
	int32 RoundToFrame() const;

	/**
	 * Retrieve a decimal representation of this frame time
	 * Sub frames are always added to the current frame number, so for negative frame times, a time of -10 [sub frame 0.25] will yield a decimal value of -9.75.
	 */
	double AsDecimal() const;

	/**
	 * Convert a decimal representation to a frame time
	 * Note that sub frames are always positive, so negative decimal representations result in an inverted sub frame and floored frame number
	 */
	static FFrameTime FromDecimal(double InDecimalFrame);

	/** IMPORTANT: If you change the struct data, ensure that you also update the version in NoExportTypes.h  */

	int32 FrameNumber;

private:

	/** Must be 0.f <= SubFrame < 1.f */
	float SubFrame;

public:

	/**
	 * Serializes the given FrameTime from or into the specified archive.
	 *
	 * @param Ar            The archive to serialize from or into.
	 * @param FrameTime     The frame time to serialize.
	 * @return The archive used for serialization.
	 */
	friend FArchive& operator<<(FArchive& Ar, FFrameTime& FrameTime)
	{
		FrameTime.Serialize(Ar);
		return Ar;
	}

	friend bool operator==(FFrameTime A, FFrameTime B)
	{
		return A.FrameNumber == B.FrameNumber && A.SubFrame == B.SubFrame;
	}


	friend bool operator!=(FFrameTime A, FFrameTime B)
	{
		return A.FrameNumber != B.FrameNumber || A.SubFrame != B.SubFrame;
	}


	friend bool operator> (FFrameTime A, FFrameTime B)
	{
		return A.FrameNumber >  B.FrameNumber || ( A.FrameNumber == B.FrameNumber && A.SubFrame > B.SubFrame );
	}


	friend bool operator>=(FFrameTime A, FFrameTime B)
	{
		return A.FrameNumber > B.FrameNumber || ( A.FrameNumber == B.FrameNumber && A.SubFrame >= B.SubFrame );
	}


	friend bool operator< (FFrameTime A, FFrameTime B)
	{
		return A.FrameNumber <  B.FrameNumber || ( A.FrameNumber == B.FrameNumber && A.SubFrame < B.SubFrame );
	}


	friend bool operator<=(FFrameTime A, FFrameTime B)
	{
		return A.FrameNumber < B.FrameNumber || ( A.FrameNumber == B.FrameNumber && A.SubFrame <= B.SubFrame );
	}


	friend FFrameTime& operator+=(FFrameTime& LHS, FFrameTime RHS)
	{
		float       NewSubFrame    = LHS.SubFrame + RHS.SubFrame;
		const int64 NewFrameNumber = int64(LHS.FrameNumber) + int64(RHS.FrameNumber) + FMath::FloorToInt(NewSubFrame);

		LHS.FrameNumber = static_cast<int32>(NewFrameNumber);
		LHS.SubFrame    = FMath::Frac(NewSubFrame);

		return LHS;
	}


	friend FFrameTime operator+(FFrameTime A, FFrameTime B)
	{
		const float NewSubFrame    = A.SubFrame + B.SubFrame;
		const int64 NewFrameNumber = static_cast<int64>(A.FrameNumber) + static_cast<int64>(B.FrameNumber) + FMath::FloorToInt(NewSubFrame);

		return FFrameTime(static_cast<int32>(NewFrameNumber), FMath::Frac(NewSubFrame));
	}


	friend FFrameTime& operator-=(FFrameTime& LHS, FFrameTime RHS)
	{
		// Ensure SubFrame is always between 0 and 1
		// Note that the difference between frame -1.5 and 1.5 is 2, not 3, since sub frame positions are always positive
		const float NewSubFrame     = LHS.SubFrame - RHS.SubFrame;
		const float FlooredSubFrame = FMath::FloorToFloat(NewSubFrame);
		const int64 NewFrameNumber  = static_cast<int64>(LHS.FrameNumber) - static_cast<int64>(RHS.FrameNumber) + FMath::TruncToInt(FlooredSubFrame);

		LHS.FrameNumber  = static_cast<int32>(NewFrameNumber);
		LHS.SubFrame = NewSubFrame - FlooredSubFrame;

		return LHS;
	}


	friend FFrameTime operator-(FFrameTime A, FFrameTime B)
	{
		// Ensure SubFrame is always between 0 and 1
		// Note that the difference between frame -1.5 and 1.5 is 2, not 3, since sub frame positions are always positive
		const float NewSubFrame     = A.SubFrame - B.SubFrame;
		const float FlooredSubFrame = FMath::FloorToFloat(NewSubFrame);
		const int64 NewFrameNumber  = static_cast<int64>(A.FrameNumber) - static_cast<int64>(B.FrameNumber) + FMath::TruncToInt(FlooredSubFrame);

		return FFrameTime(static_cast<int32>(NewFrameNumber), NewSubFrame - FlooredSubFrame);
	}


	friend FFrameTime operator%(FFrameTime A, FFrameTime B)
	{
		if (A.SubFrame == 0.f && B.SubFrame == 0.f)
		{
			return FFrameTime(A.FrameNumber % B.FrameNumber);
		}
		else
		{
			FFrameTime Result = A;
			while (Result >= B)
			{
				Result = Result - B;
			}
			return Result;
		}
	}


	friend FFrameTime operator-(FFrameTime A)
	{
		return A.GetSubFrame() == 0.f
			? FFrameTime(-A.FrameNumber)
			: FFrameTime(-A.FrameNumber - 1, 1.f-A.GetSubFrame());
	}


	friend FORCEINLINE FFrameTime operator*(FFrameTime A, double Scalar)
	{
		return FFrameTime::FromDecimal(A.AsDecimal() * Scalar);
	}

	friend FORCEINLINE FFrameTime operator*(double Scalar, FFrameTime A)
	{
		return FFrameTime::FromDecimal(A.AsDecimal() * Scalar);
	}

	friend FORCEINLINE FFrameTime operator/(FFrameTime A, double Scalar)
	{
		return FFrameTime::FromDecimal(A.AsDecimal() / Scalar);
	}
};


inline FFrameTime::FFrameTime()
	: FrameNumber(0), SubFrame(0.f)
{}


template<typename T, typename>
FFrameTime::FFrameTime(T InFrameNumber)
	: FrameNumber(InFrameNumber), SubFrame(0.f)
{}


inline FFrameTime::FFrameTime(int32 InFrameNumber)
	: FrameNumber(InFrameNumber), SubFrame(0.f)
{}


inline FFrameTime::FFrameTime(int32 InFrameNumber, float InSubFrame)
	: FrameNumber(InFrameNumber), SubFrame(InSubFrame)
{
	// Hack to ensure that SubFrames are in a sensible range of precision to work around
	// problems with FloorToXYZ returning the wrong thing for very small negative numbers
	SubFrame = FMath::Clamp(SubFrame + 0.5f - 0.5f, 0.f, MaxSubframe);
}


inline bool FFrameTime::Serialize(FArchive& Ar)
{
	Ar << FrameNumber;
	Ar << SubFrame;
	return true;
}


inline FFrameTime& FFrameTime::operator=(int32 InFrameNumber)
{
	FrameNumber = InFrameNumber;
	SubFrame    = 0.f;
	return *this;
}

inline int32 FFrameTime::FloorToFrame() const
{
	return FrameNumber;
}


inline int32 FFrameTime::CeilToFrame() const
{
	return SubFrame == 0.f ? FrameNumber : FrameNumber+1;
}


inline int32 FFrameTime::RoundToFrame() const
{
	return SubFrame < .5f ? FrameNumber : FrameNumber+1;
}


inline double FFrameTime::AsDecimal() const
{
	return static_cast<double>(FrameNumber) + SubFrame;
}

inline FFrameTime FFrameTime::FromDecimal(double InDecimalFrame)
{
	int32 NewFrame = static_cast<int32>(
	    FMath::Clamp(
	        FMath::FloorToDouble(InDecimalFrame),
	        static_cast<double>(TNumericLimits<int32>::Min()),
	        static_cast<double>(TNumericLimits<int32>::Max())
	    )
	);

	// Ensure fractional parts above the highest sub frame float precision do not round to 0.0
	double Fraction = InDecimalFrame - FMath::FloorToDouble(InDecimalFrame);
	return FFrameTime(NewFrame, FMath::Clamp(static_cast<float>(Fraction), 0.0f, MaxSubframe));
}

/** Convert a FFrameTime into a string */
inline FString LexToString(const FFrameTime InTime)
{
	return FString::Printf(TEXT("Frame: %d Subframe: %f"), InTime.GetFrame(), InTime.GetSubFrame());
}
