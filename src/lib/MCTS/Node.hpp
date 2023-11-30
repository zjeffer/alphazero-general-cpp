#pragma once

#include <memory>
#include <vector>

#include "../Environment/Environment.hpp"

class Node
{
private:
  std::vector<std::unique_ptr<Node>> m_children;          // children of this node
  std::shared_ptr<Environment>       m_environment;       // environment at this node
  std::shared_ptr<Node>              m_parent;            // parent of this node (nullptr if root)
  std::shared_ptr<Move>              m_move;              // move that led to this node
  float                              m_priorProbability;  // prior probability of the move that led to this node
  uint                               m_visitCount = 0;    // number of times this node has been visited // TODO: should this be 1 at construction?
  float                              m_value      = 0.0F; // value of this node

public:
  Node(std::shared_ptr<Environment> environment, std::shared_ptr<Node> parent, std::shared_ptr<Move> move);
  Node(std::shared_ptr<Environment> environment);
  ~Node() = default;

  std::shared_ptr<Environment> const & GetEnvironment() const;

  std::shared_ptr<Node> const & GetParent() const;

  Move const & GetMove() const;

  float GetPriorProbability() const;
  void  SetPriorProbability(float priorProbability);

  bool                                       IsLeaf() const;
  std::vector<std::unique_ptr<Node>> const & GetChildren() const;
  void                                       AddChild(std::unique_ptr<Node> child);

  uint GetVisitCount() const;
  void SetVisitCount(uint visitCount);
  void IncrementVisitCount();

  float GetQValue() const;
  float GetUValue() const;

  float GetValue() const;
  void  SetValue(float value);
};