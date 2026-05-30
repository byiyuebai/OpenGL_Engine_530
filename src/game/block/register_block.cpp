#include "register_block.h"
#include <iostream>

RegisterBlock* RegisterBlock::instance = nullptr;
std::mutex RegisterBlock::mtx;

RegisterBlock::RegisterBlock()
{
	blockIDCount = 10; // 从 10 开始注册，0-9 预留
	registerBlock("air", 0);
	registerBlock("error", 2);
	registerBlock("bricks_my");
	registerBlock("stone");
	registerBlock("bricks_mc");
	registerBlock("grass_up");
	//registerBlock("grass");
	registerBlock("red");
}

void RegisterBlock::registerBlock(const std::string& name,unsigned int id)
{
	if (registerBlocks.find(name) != registerBlocks.end()) {
		// 已注册，直接返回
		std::wcerr << L"\033[31m[ERROR]\033[0m [register_block]方块已注册,或者名称重复:" << name.c_str() << std::endl;
		return;
	}
	if (id == 1) {
		// 注册新方块，分配新的 ID
		BlockState newState{ blockIDCount, new BlockData{name, nullptr} };
		registerBlocks.insert({ name, newState });
		registerBlocksById.insert({ blockIDCount, newState });
		blockIDCount++;
	} else {
		// 使用指定 ID 注册方块
		BlockState newState{ id, new BlockData{name, nullptr} };
		registerBlocks.insert({ name, newState });
		registerBlocksById.insert({ id, newState });
	}
}

BlockState RegisterBlock::getBlockState_NAME(const std::string& name) const
{
	auto it = registerBlocks.find(name);
	if (it != registerBlocks.end()) {
		return it->second;
	}
	//std::wcerr << L"方块未注册:" << name.c_str() << std::endl;
	return getBlockState_NAME("error");
}

BlockState RegisterBlock::getBlockState_ID(int id) const
{
	auto it = registerBlocksById.find(id);
	if (it != registerBlocksById.end()) {
		return it->second;
	}
	//std::wcerr << L"方块未注册:" << id << std::endl;
	return getBlockState_NAME("error");
}
