#include "Node.hpp"

Node::Node(std::shared_ptr<Environment> environment, std::shared_ptr<Move> move)
  : m_environment(std::move(environment))
  , m_move(std::move(move))
{
}

Node::Node(std::shared_ptr<Environment> environment)
  : m_environment(std::move(environment))
{
  // constructor for root node
}

std::shared_ptr<Environment> const & Node::GetEnvironment() const
{
  return m_environment;
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
