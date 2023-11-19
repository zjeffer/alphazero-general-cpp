#pragma once

#include <memory>
#include <vector>

#include "../Environment/Environment.hpp"

class Node
{
private:
  std::vector<std::unique_ptr<Node>> m_children;
  std::shared_ptr<Environment>       m_environment;
  std::shared_ptr<Move>              m_move;

  uint m_visitCount;

public:
  Node(std::shared_ptr<Environment> environment, std::shared_ptr<Move> move);
  Node(std::shared_ptr<Environment> environment);
  ~Node() = default;

  std::shared_ptr<Environment> const & GetEnvironment() const;

  bool                                       IsLeaf() const;
  std::vector<std::unique_ptr<Node>> const & GetChildren() const;
  void                                       AddChild(std::unique_ptr<Node> child);

  uint  GetVisitCount() const;
  float GetQValue() const;
  float GetUValue() const;
};