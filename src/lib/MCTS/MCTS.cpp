#include "MCTS.hpp"

#include "../../lib/Logging/Logger.hpp"
#include "../../lib/Utilities/RandomGenerator.hpp"
#include "../../lib/Utilities/tqdm.hpp"

MCTS::MCTS(std::shared_ptr<Node> root, DirichletNoiseOptions const & dirichletNoiseOptions)
  : m_root(std::move(root))
  , m_dirichletNoiseOptions(dirichletNoiseOptions)
{
}

void MCTS::RunSimulations(uint numSimulations, NeuralNetworkInterface & network)
{
  LINFO << "Running " << numSimulations << " simulations";
  std::shared_ptr<Node> root = GetRoot();

  try
  {
    if (m_dirichletNoiseOptions.enable)
    {
      Expand(root, network); // expand the root node once, so we can add dirichlet noise to it
      AddDirichletNoiseToRoot();
    }
  }
  catch (std::exception const & e)
  {
    LWARN << "Exception while adding dirichlet noise to root: " << e.what();
    throw std::runtime_error("Exception while adding dirichlet noise to root: " + std::string(e.what()));
  }

  // run simulations
  try
  {
    tqdm bar;
    for (uint i = 0; i < numSimulations; i++)
    {
      bar.progress(i, numSimulations);
      // 1. select
      std::shared_ptr<Node> leafNode = Select(root);
      // 2. expand and 3. evaluate
      float result = Expand(leafNode, network);
      // 4. backpropagate
      Backpropagate(leafNode, result);
    }
    bar.finish();
  }
  catch (std::exception const & e)
  {
    LWARN << "Exception while running simulations: " << e.what();
    throw std::runtime_error("Exception while running simulations: " + std::string(e.what()));
  }

  LINFO << "Finished running simulations, tree depth: " << GetTreeDepth(root.get());
}

std::shared_ptr<Node> MCTS::GetRoot() const
{
  return m_root;
}

std::shared_ptr<Node> MCTS::Select(std::shared_ptr<Node> const & root)
{
  // select nodes until we reach a leaf node (= a node that has not been expanded yet)
  // do the selection using the Q+U formula
  std::shared_ptr<Node> current = root;

  uint depth = 0;
  while (!current->IsLeaf())
  {
    depth++;
    float                 bestScore = -INFINITY;
    std::shared_ptr<Node> bestChild = nullptr;
    for (auto const & child: current->GetChildren())
    {
      float score = child->GetQValue() + child->GetUValue();
      if (score > bestScore)
      {
        bestScore = score;
        bestChild = child;
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

float MCTS::Expand(std::shared_ptr<Node> const & node, NeuralNetworkInterface & network)
{
  // create all possible child nodes
  // 1. convert the node to an input usable by the neural network
  auto input = node->GetEnvironment()->BoardToInput();
  // 2. run the neural network's predict function
  auto [policyOutput, valueOutput] = network.Predict(input);
  // reshape policyOutput to the same shape as the input
  policyOutput = policyOutput.view({input.sizes()[1], input.sizes()[2]});
  // 3. create a child node for each possible move in the policy output, and add them to the node
  auto validMoves = node->GetEnvironment()->GetValidMoves();

  auto winner = node->GetEnvironment()->GetWinner();
  if (validMoves.empty() || winner != Player::PLAYER_NONE)
  {
    if (winner == Player::PLAYER_NONE)
    {
      return 0.0F;
    }
    auto currentPlayer = node->GetEnvironment()->GetCurrentPlayer();
    // LINFO << "No more valid moves for player " << node->GetEnvironment()->PlayerToString(currentPlayer)
    //       << ". Winner: " << node->GetEnvironment()->PlayerToString(winner);
    if (winner == currentPlayer)
    {
      // if the winner of the this leaf node's board is the current player
      // then the opponent made the move that led to this winning board
      return -1.0F;
    }
    // else the current player made the move that led to this winning board
    return 1.0F;
  }

  for (auto const & move: validMoves)
  {
    // get the prior from the policy output
    move->SetPriorProbability(policyOutput[move->GetRow()][move->GetColumn()].item<float>());
    // create a new environment with this move
    auto newEnvironment = std::shared_ptr<Environment>(node->GetEnvironment()->Clone());
    newEnvironment->MakeMove(*move);
    // create a new node with this environment
    auto newNode = std::make_shared<Node>(std::move(newEnvironment), node, move);
    // add a new node to the current node with this prior
    node->AddChild(std::move(newNode));
  }
  // 4. return the value output
  // = the value of the leaf node, assuming the current player has to make a move
  return valueOutput.view(1).item<float>();
}

void MCTS::Backpropagate(std::shared_ptr<Node> const & node, float reward)
{
  auto currentPlayer = node->GetEnvironment()->GetCurrentPlayer();
  // starting from the given leaf node, go up the tree and update the visit counts and values
  Node * currentNode = node.get();
  while (currentNode->GetParent() != nullptr)
  {
    currentNode->IncrementVisitCount();
    if (currentNode->GetEnvironment()->GetCurrentPlayer() == currentPlayer)
    {
      currentNode->SetValue(currentNode->GetValue() + reward);
    }
    else
    {
      currentNode->SetValue(currentNode->GetValue() - reward);
    }
    currentNode = currentNode->GetParent().get();
  }
  currentNode->IncrementVisitCount(); // root node
}

uint MCTS::GetTreeDepth(Node * root)
{
  // recursive function of getting height of the tree
  if (root->IsLeaf())
  {
    return 0;
  }
  int maxDepth = 0;
  for (auto const & child: root->GetChildren())
  {
    int depth = MCTS::GetTreeDepth(child.get());
    if (depth > maxDepth)
    {
      maxDepth = depth;
    }
  }
  return maxDepth + 1;
}

void MCTS::AddDirichletNoiseToRoot() const
{
  auto const & children = GetRoot()->GetChildren();
  if (children.empty())
  {
    LWARN << "No children found, cannot add dirichlet noise";
  }

  // get the prior probabilities
  std::vector<float> priorProbabilities;
  priorProbabilities.reserve(children.size());
  for (auto const & child: children)
  {
    priorProbabilities.emplace_back(child->GetMove()->GetPriorProbability());
  }

  // add dirichlet noise to the prior probabilities of the root node
  std::vector<float> dirichletNoise = RandomGenerator::CalculateDirichletNoise(priorProbabilities,
                                                                               m_dirichletNoiseOptions.alpha,
                                                                               m_dirichletNoiseOptions.beta,
                                                                               m_dirichletNoiseOptions.dirichletFraction);
  for (size_t i = 0; i < children.size(); i++)
  {
    children[i]->GetMove()->SetPriorProbability(dirichletNoise[i]);
  }
}

std::shared_ptr<Move> MCTS::GetBestMove(bool stochasticSearch) const
{
  // get the best move from the root node
  // if stochasticSearch is true, use the visit counts as probabilities
  // if stochasticSearch is false, use the highest visit count
  return stochasticSearch ? GetBestMoveStochastic() : GetBestMoveDeterministic();
}

std::shared_ptr<Move> MCTS::GetBestMoveStochastic() const
{
  auto const & children = GetRoot()->GetChildren();
  if (children.empty())
  {
    throw std::runtime_error("No children found while getting best move");
  }
  // get the visit counts
  std::vector<float> visitCounts;
  visitCounts.reserve(children.size());
  for (auto const & child: children)
  {
    visitCounts.emplace_back(child->GetVisitCount());
  }
  // normalize the visit counts
  float sum = std::accumulate(visitCounts.begin(), visitCounts.end(), 0.0F);
  for (auto & visitCount: visitCounts)
  {
    visitCount /= sum;
  }
  // sample from the visit counts
  uint stochasticIndex = RandomGenerator::StochasticSample(visitCounts);
  auto bestChild       = children[stochasticIndex];
  return bestChild->GetMove();
}

std::shared_ptr<Move> MCTS::GetBestMoveDeterministic() const
{
  auto const & children = GetRoot()->GetChildren();
  if (children.empty())
  {
    throw std::runtime_error("No children found while getting best move");
  }
  // find the child with the highest visit count
  auto bestChild = std::max_element(                                                           //
    children.begin(),                                                                          //
    children.end(),                                                                            //
    [](auto const & c1, auto const & c2) { return c1->GetVisitCount() < c2->GetVisitCount(); } //
  );

  if (bestChild == children.end())
  {
    throw std::runtime_error("No best child found");
  }
  return bestChild->get()->GetMove();
}