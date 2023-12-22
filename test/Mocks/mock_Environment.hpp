#pragma once

#include <gmock/gmock.h>

#include "../../src/lib/Environment/Environment.hpp"

using ::testing::_;

class MockEnvironment : public Environment
{
public:
  MockEnvironment()
  {
    EXPECT_CALL(*this, BoardToInput()).Times(::testing::AnyNumber());
    EXPECT_CALL(*this, GetBoard()).Times(::testing::AnyNumber());
    EXPECT_CALL(*this, SetBoard(_, _)).Times(::testing::AnyNumber());
  }

  MOCK_METHOD(std::unique_ptr<Environment>, Clone, (), (const, override));

  MOCK_METHOD(Player, GetCurrentPlayer, (), (const, override));
  MOCK_METHOD(void, SetCurrentPlayer, (Player player), (override));
  MOCK_METHOD(void, TogglePlayer, (), (override));

  MOCK_METHOD(void, MakeMove, (Move const & move), (override));
  MOCK_METHOD(void, UndoMove, (), (override));
  MOCK_METHOD(bool, IsValidMove, (uint row, uint column), (const, override));

  MOCK_METHOD(std::vector<std::shared_ptr<Move>>, GetValidMoves, (), (const, override));
  MOCK_METHOD(std::vector<std::shared_ptr<Move>> const &, GetMoveHistory, (), (const, override));

  MOCK_METHOD(int, GetRows, (), (const, override));
  MOCK_METHOD(int, GetColumns, (), (const, override));

  MOCK_METHOD(Player, GetPlayerAtCoordinates, (uint row, uint column), (const, override));

  MOCK_METHOD(torch::Tensor const &, GetBoard, (), (const, override));
  MOCK_METHOD(void, SetBoard, (torch::Tensor const & board, Player currentPlayer), (override));

  MOCK_METHOD(torch::Tensor, BoardToInput, (), (const, override));

  MOCK_METHOD(bool, IsTerminal, (), (const, override));
  MOCK_METHOD(Player, GetWinner, (), (const, override));

  MOCK_METHOD(void, PrintBoard, (), (const, override));

  MOCK_METHOD(void, ResetEnvironment, (), (override));

  MOCK_METHOD(std::string, PlayerToString, (Player player), (const, override));
};
