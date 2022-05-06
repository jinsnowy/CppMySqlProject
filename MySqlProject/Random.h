#pragma once
#ifndef  _RANDOM_H_
#define _RANDOM_H_

#include <iostream>
#include <array>
#include <cstdio>
#include <cassert>
#include <memory>
#include <random>
#include <unordered_set>

#include "DateTime.h"

class RandomDevice
{
private:
	std::random_device device;
	std::unique_ptr<std::seed_seq> seeds;
public:
	RandomDevice()
	{
		auto seed_data = std::array<int, std::mt19937::state_size>{};
		std::generate(std::begin(seed_data), std::end(seed_data), std::ref(device));
		seeds = std::make_unique<std::seed_seq>(std::begin(seed_data), std::end(seed_data));
	}

	std::mt19937 Generate()
	{
		return std::mt19937(*seeds);
	}

	static RandomDevice& Get()
	{
		static RandomDevice device;
		return device;
	}
};

class Random
{
private:
	std::mt19937 engine;
	std::uniform_int_distribution<int> int_dist;

public:
	Random()
		:
		engine(RandomDevice::Get().Generate())
	{
	}

	Random(int minValue, int maxValue)
		:
		int_dist(minValue, maxValue)
	{}

	static Random GetRandom(int minValue, int maxValue)
	{
		return Random(minValue, maxValue);
	}

	template<int minValue, int maxValue>
	static Random GetRandom()
	{
		return Random(minValue, maxValue);
	}

	int Next()
	{
		return int_dist(engine);
	}

	std::vector<int> GetRandomUniqueSet(unsigned int nSet)
	{
		std::unordered_set<int> set;
		while (set.size() < nSet)
		{
			int select = Next();
			if (set.find(select) == set.end())
			{
				set.insert(Next());
			}
		}

		return std::vector<int>(set.begin(), set.end());
	}

	static DateTime GetRandomDateTime()
	{
		static Random year_rnd = Random::GetRandom<80, 125>();
		static Random month_rnd = Random::GetRandom<0, 11>();
		static Random days_rnd = Random::GetRandom<1, 31>();
		static Random hours_rnd = Random::GetRandom<0, 23>();
		static Random min_rnd = Random::GetRandom<0, 59>();
		static Random sec_rnd = Random::GetRandom<0, 59>();

		DateTime date = {};
		do
		{
			date.tm_year = year_rnd.Next();
			date.tm_mon = month_rnd.Next();
			date.tm_mday = days_rnd.Next();
			date.tm_hour = hours_rnd.Next();
			date.tm_min = min_rnd.Next();
			date.tm_sec = sec_rnd.Next();
		} while (date.IsValid() == false);

		mktime((tm*)&date);

		return date;
	}
};

#endif // ! _RANDOM_H_
