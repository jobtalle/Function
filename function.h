#pragma once

#include <stdint.h>

#define FUNCTION_MAX_PARTS 4

enum functionType {
	FUNCTION_LINEAR,
	FUNCTION_POWER,
	FUNCTION_SINE,
	FUNCTION_CONSTANT,
	FUNCTION_VOID
};

enum funcitonPartType {
	FUNCTION_PART_BASE,
	FUNCTION_PART_MUL,
	FUNCTION_PART_ADD,
	FUNCTION_PART_SUB,
	FUNCTION_PART_DIV,
	FUNCTION_PART_CLAMP
};

typedef struct {
	enum functionType type;
	enum funcitonPartType part;

	union {
		struct {
			float low;
			float high;
		} clamp;

		struct {
			float scale;
			float offset;
			float amplitude;
			float exp;
		} power;

		struct {
			float scale;
			float offset;
			float slope;
		} linear;

		struct {
			float periods;
			float offset;
			float amplitude;
		} sine;

		struct {
			float value;
		} constant;
	};
} functionPart;

typedef struct {
	uint8_t partCount;
	functionPart parts[FUNCTION_MAX_PARTS];
} function;

float functionSample(const function f, const float x);

function functionClamp(function f, const float low, const float high);
function functionMultiply(function a, function b);
function functionAdd(function a, function b);
function functionSubtract(function a, function b);
function functionDivide(function a, function b);

function functionCreateLinear(
	const float scale,
	const float offset,
	const float slope);
function functionCreatePower(
	const float scale,
	const float offset,
	const float amplitude,
	const float exp);
function functionCreateSine(
	const float periods,
	const float offset,
	const float amplitude);
function functionCreateConstant(
	const float value);
function functionCreateVoid();