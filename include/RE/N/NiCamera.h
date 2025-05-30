#pragma once

#include "RE/N/NiFrustum.h"
#include "RE/N/NiPoint.h"
#include "RE/N/NiRect.h"

namespace RE
{
	class __declspec(novtable) NiCamera :
		public NiAVObject
	{
	public:
		SF_RTTI_VTABLE(NiCamera);

		virtual ~NiCamera() = default;

		//leftBoundary and rightBoundary are outputted based on the W (radius) component of worldPt.
		//If the W component is small enough, they are essentially equal, and point to the exact
		//XYZ location provided.
		//The returned NiPoint2 does not contain any world-to-screen information and can be ignored.
		NiPoint2 WorldToScreenInternal(const NiPoint4* a_worldPt, NiPoint3* a_leftBoundary, NiPoint3* a_rightBoundary)
		{
			using func_t = decltype(&NiCamera::WorldToScreenInternal);
			static REL::Relocation<func_t> func{ ID::NiCamera::WorldToScreenInternal };
			return func(this, a_worldPt, a_leftBoundary, a_rightBoundary);
		}

		//The returned X and Y are in the range -1 to 1, with 1 being top right and -1 being bottom left.
		//The returned Z is the distance from the camera to the worldPt, with negative values meaning the
		//camera is facing away from the worldPt.
		//Note: The Z component is very small due to Starfield's unit scale.
		//i.e. distance from player to camera when the camera is close is ~0.0004
		NiPoint3 WorldToScreen(const NiPoint3& a_worldPt)
		{
			NiPoint3 result{ 0.0f, 0.0f, -1.0f };
			float    worldDiffRotated = (((a_worldPt.y - world.translate.y) * world.rotate[0][1]) +
                                         ((a_worldPt.x - world.translate.x) * world.rotate[0][0]) +
                                         ((a_worldPt.z - world.translate.z) * world.rotate[0][2])) -
			                         viewFrustum.near;

			result.z = worldDiffRotated * (1.0f / (viewFrustum.far - viewFrustum.near));

			float trace = (a_worldPt.x * worldToCam[3][0]) + (a_worldPt.y * worldToCam[3][1]) + ((a_worldPt.z * worldToCam[3][2]) + worldToCam[3][3]);
			if (trace <= 0.00001f) {
				return result;
			}

			float traceInv = 1.0f / trace;
			result.x = (((a_worldPt.y * worldToCam[0][1]) + (a_worldPt.x * worldToCam[0][0])) + ((a_worldPt.z * worldToCam[0][2]) + worldToCam[0][3])) * traceInv;
			result.y = (((a_worldPt.y * worldToCam[1][1]) + (a_worldPt.x * worldToCam[1][0])) + ((a_worldPt.z * worldToCam[1][2]) + worldToCam[1][3])) * traceInv;
			return result;
		}

		//Same as WorldToScreen, but normalizes X and Y to the 0 to 1 range,
		//and flips the Y component so that 0 is top left and 1 is bottom right.
		NiPoint3 WorldToScreenNormalized(const NiPoint3& a_worldPt)
		{
			auto result = WorldToScreen(a_worldPt);
			result.x = (result.x + 1.0f) * 0.5f;
			result.y = -result.y;
			result.y = (result.y + 1.0f) * 0.5f;
			return result;
		}

		// members
		float         unk[20];
		float         worldToCam[4][4];
		NiFrustum     viewFrustum;
		float         minNearPlaneDist;
		float         maxFarNearRatio;
		NiRect<float> port;
		float         lodAdjust;
		float         unk2[24];
	};
	// static_assert(offsetof(NiCamera, NiCamera::worldToCam) == 384); // FIXME: clang-cl chokes on this assertion
}
