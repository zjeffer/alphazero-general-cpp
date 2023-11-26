#include "Node.hpp"

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
  // TODO
  return 0.0F;
}

float Node::GetUValue() const
{
  // TODO
  return 0.0F;
}
