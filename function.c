#include <math.h>
#include <stdlib.h>

#include "function.h"

#ifdef _DEBUG
#include <assert.h>
#endif

static float functionPartSample(const functionPart p, const float x)
{
	switch(p.type) {
	case FUNCTION_LINEAR:
		return (x * p.linear.scale + p.linear.offset) * p.linear.slope;
	case FUNCTION_POWER:
		return powf((x * p.power.scale + p.power.offset), p.power.exp) * p.power.amplitude;
	case FUNCTION_SINE:
		return sinf(6.283185f * (x * p.sine.periods - p.sine.offset)) * p.sine.amplitude;
	default:
	case FUNCTION_CONSTANT:
		return p.constant.value;
	}
}

static function functionFromPart(functionPart p)
{
	function f;

	p.part = FUNCTION_PART_BASE;

	f.partCount = 1;
	f.parts[0] = p;

	return f;
}

static float functionSamplePart(const functionPart p, const float intermediate, const float x)
{
#ifdef _DEBUG
	assert(p.type != FUNCTION_VOID);
#endif

	switch(p.part) {
	default:
	case FUNCTION_PART_BASE:
		return functionPartSample(p, x);
	case FUNCTION_PART_MUL:
		return intermediate * functionPartSample(p, x);
	case FUNCTION_PART_ADD:
		return intermediate + functionPartSample(p, x);
	case FUNCTION_PART_SUB:
		return intermediate - functionPartSample(p, x);
	case FUNCTION_PART_DIV:
		return intermediate / functionPartSample(p, x);
	case FUNCTION_PART_CLAMP:
		return max(p.clamp.low, min(p.clamp.high, intermediate));
	}
}

float functionSample(const function f, const float x)
{
	uint8_t i;
	float intermediate = 0;

	for(i = 0; i < f.partCount; ++i)
		intermediate = functionSamplePart(f.parts[i], intermediate, x);

	return intermediate;
}

function functionClamp(function f, const float low, const float high)
{
	functionPart p;

	p.part = FUNCTION_PART_CLAMP;
	p.clamp.low = low;
	p.clamp.high = high;

#ifdef _DEBUG
	assert(f.partCount + 1 <= FUNCTION_MAX_PARTS);
#endif

	f.parts[f.partCount++] = p;

	return f;
}

static function functionCombine(function a, function b, enum functionPartType part)
{
	uint8_t i;
#ifdef _DEBUG
	assert(a.partCount + b.partCount <= FUNCTION_MAX_PARTS);
#endif

	b.parts[0].part = part;

	for(i = 0; i < b.partCount; ++i)
		a.parts[a.partCount++] = b.parts[i];

	return a;
}

function functionMultiply(function a, function b)
{
	return functionCombine(a, b, FUNCTION_PART_MUL);
}

function functionAdd(function a, function b)
{
	return functionCombine(a, b, FUNCTION_PART_ADD);
}

function functionSubtract(function a, function b)
{
	return functionCombine(a, b, FUNCTION_PART_SUB);
}

function functionDivide(function a, function b)
{
	return functionCombine(a, b, FUNCTION_PART_DIV);
}

function functionCreateLinear(
	const float scale,
	const float offset,
	const float slope)
{
	functionPart p;

	p.type = FUNCTION_LINEAR;
	p.linear.scale = scale;
	p.linear.offset = offset;
	p.linear.slope = slope;

	return functionFromPart(p);
}

function functionCreatePower(
	const float scale,
	const float offset,
	const float amplitude,
	const float exp)
{
	functionPart p;

	p.type = FUNCTION_POWER;
	p.power.scale = scale;
	p.power.offset = offset;
	p.power.amplitude = amplitude;
	p.power.exp = exp;

	return functionFromPart(p);
}

function functionCreateSine(
	const float periods,
	const float offset,
	const float amplitude)
{
	functionPart p;

	p.type = FUNCTION_SINE;
	p.sine.periods = periods;
	p.sine.offset = offset;
	p.sine.amplitude = amplitude;

	return functionFromPart(p);
}

function functionCreateConstant(
	const float value)
{
	functionPart p;

	p.type = FUNCTION_CONSTANT;
	p.constant.value = value;

	return functionFromPart(p);
}

function functionCreateVoid()
{
	functionPart p;

	p.type = FUNCTION_VOID;

	return functionFromPart(p);
}