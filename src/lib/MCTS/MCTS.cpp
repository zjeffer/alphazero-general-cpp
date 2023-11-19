#include "MCTS.hpp"

#include "../../lib/Utilities/tqdm.hpp"

MCTS::MCTS(std::shared_ptr<Node> root)
  : m_root(std::move(root))
{
}

void MCTS::RunSimulations(uint numSimulations, NeuralNetwork & network)
{
  Node * root = GetRoot();

  // TODO: add dirichlet noise to the root node

  tqdm bar;
  for (uint i = 0; i < numSimulations; i++)
  {
    bar.progress(i, numSimulations);
    // 1. select
    Node * node = Select(root);
    // 2. expand
    float result = Expand(node, network);
    // 3. evaluate
    (void)result;
    // 4. backpropagate
  }
}

Node * MCTS::GetRoot() const
{
  return m_root.get();
}

Node * MCTS::Select(Node * root)
{
  // select nodes until we reach a leaf node (= a node that has not been expanded yet)
  // do the selection using the Q+U formula
  Node * current   = root;
  float  bestScore = -INFINITY;
  while (!current->IsLeaf())
  {
    Node * bestChild = nullptr;
    for (auto const & child: current->GetChildren())
    {
      float score = child->GetQValue() + child->GetUValue();
      if (score > bestScore)
      {
        bestScore = score;
        bestChild = child.get();
      }
    }
    if (bestChild == nullptr)
    {
      throw std::runtime_error("No best child found");
    }
    current = bestChild;
  }
  return current;
}

float MCTS::Expand(Node * node, NeuralNetwork & network)
{
  // create all possible child nodes
  // 1. convert the node to an input usable by the neural network
  auto input = node->GetEnvironment()->BoardToInput();
  // 2. run the neural network's predict function
  auto [policyOutput, valueOutput] = network.Predict(input);
  // 3. create a child node for each possible move in the policy output, and add them to the node
  auto validMoves = node->GetEnvironment()->GetValidMoves();
  for (auto const & move: validMoves)
  {
    // get the prior from the policy output
    move->SetPriorProbability(policyOutput[move->GetRow()][move->GetColumn()].item<float>());
    // add a new node to the current node with this prior
    node->AddChild(std::make_unique<Node>(node->GetEnvironment(), move));
  }
  // 4. return the value output
  return valueOutput.view(1).item<float>();
}