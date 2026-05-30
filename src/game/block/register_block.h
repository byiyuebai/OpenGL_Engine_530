#pragma once
#include <mutex>
#include <unordered_map>
#include "block_best.h"
class RegisterBlock {

private:
	static RegisterBlock* instance;
	static std::mutex mtx;

	RegisterBlock();

	RegisterBlock(const RegisterBlock&) = delete;
	RegisterBlock& operator=(const RegisterBlock&) = delete;

	unsigned int blockIDCount; // 已注册的方块数量
	std::unordered_map<std::string, BlockState> registerBlocks;	// 存储映射关系
	std::unordered_map<int, BlockState> registerBlocksById;


public:
	static RegisterBlock* getInstance() {
		// 第一层检查：避免每次调用都加锁（提升性能）
		if (instance == nullptr) {
			std::lock_guard<std::mutex> lock(mtx); // 加锁（自动释放，避免死锁）
			// 第二层检查：防止多线程同时通过第一层检查后重复创建
			if (instance == nullptr) {
				instance = new RegisterBlock();
			}
		}return instance;
	}

	void registerBlock(const std::string& name, unsigned int id = 1);
	BlockState getBlockState_NAME(const std::string& name) const;
	BlockState getBlockState_ID(int id) const;

};