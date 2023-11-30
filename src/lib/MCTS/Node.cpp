#include "Node.hpp"

#include <cmath>

namespace
{
auto constexpr PB_C_BASE = 19652.0F;
auto constexpr PB_C_INIT = 1.25F;
auto constexpr C_PUCT    = 1.25F; // PUCT constant: higher -> more exploration, lower -> more exploitation
} // namespace

Node::Node(std::shared_ptr<Environment> environment, std::shared_ptr<Node> parent, std::shared_ptr<Move> move)
  : m_environment(std::move(environment))
  , m_parent(std::move(parent))
  , m_move(std::move(move))
  , m_priorProbability(m_move->GetPriorProbability())
{
}

Node::Node(std::shared_ptr<Environment> environment)
  : m_environment(std::move(environment))
  , m_move(nullptr)
  , m_priorProbability(0.0F)
{
  // constructor for root node
}

std::shared_ptr<Environment> const & Node::GetEnvironment() const
{
  return m_environment;
}

Move const & Node::GetMove() const
{
  if (m_move == nullptr)
  {
    throw std::runtime_error("Move is null");
  }
  return *m_move;
}

bool Node::IsLeaf() const
{
  return m_children.empty();
}

std::vector<std::unique_ptr<Node>> const & Node::GetChildren() const
{
  return m_children;
}

void Node::AddChild(std::unique_ptr<Node> child)
{
  m_children.emplace_back(std::move(child));
}

uint Node::GetVisitCount() const
{
  return m_visitCount;
}

void Node::SetVisitCount(uint visitCount)
{
  m_visitCount = visitCount;
}

void Node::IncrementVisitCount()
{
  m_visitCount++;
}

float Node::GetPriorProbability() const
{
  return m_priorProbability;
}

void Node::SetPriorProbability(float priorProbability)
{
  m_priorProbability = priorProbability;
}

float Node::GetQValue() const
{
  return m_value / ((float)GetVisitCount() + 1e-3);
}

float Node::GetUValue() const
{
  if (m_parent == nullptr)
  {
    throw std::runtime_error("Parent is null. This shouldn't happen unless you call this function on the root node.");
  }
  // uses the PUCT formula based on AlphaZero's paper and pseudocode
  float expRate = std::log((m_parent->GetVisitCount() + PB_C_BASE + 1.0F) / PB_C_BASE) + PB_C_INIT;
  expRate *= std::sqrt((float)m_parent->GetVisitCount()) / ((float)GetVisitCount() + 1.0F);
  return C_PUCT * expRate * m_priorProbability;
}

float Node::GetValue() const
{
  return m_value;
}

void Node::SetValue(float value)
{
  m_value = value;
}
