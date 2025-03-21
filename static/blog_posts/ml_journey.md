# Machine Learning conditioned on me :)

*Posted: July 1, 2024*

## Where I started

My journey into machine learning began in late high school when I got into bot-building for PC games that I played. I was motivated by the benefits it provided me in the game, but quickly realized that the same principles could be applied to other problems. In college, I was fascinated by the concept of teaching computers to learn from data and make decisions. The theoretical foundations of ML - from linear algebra to calculus to probability theory - provided a robust framework that appealed to my analytical mind.

## Early Projects

My first significant long-term project involved developing a reinforcement learning algorithm for robotic object grasping and manipulation. Working in the Robot Perception and Action Laboratory (RPAL), I trained a PPO agent that allowed a Barrett Hand to learn grasping strategies through trial and error. From my interest in this area, I developed my own implementation of the Actor-Critic algorithm [here](https://github.com/theCoderSchoolTampa/CoderSchoolAI/blob/master/CoderSchoolAI/Neural/ActorCritic/ActorCriticNetwork.py). Implementing this algorithm from scratch makes it so you understand exactly how the signal propagates through the network. Here's a simple implementation of the Actor-Critic algorithm:

### Actor-Critic 
```python
import torch
import torch.nn as nn

class ActorCritic(nn.Module):
    def __init__(self, input_dim, action_dim):
        super(ActorCritic, self).__init__()
        self.shared_layers = nn.Sequential(
            nn.Linear(input_dim, 128),
            nn.ReLU(),
            nn.Linear(128, 64),
            nn.ReLU()
        )
        
        # Actor head
        self.actor = nn.Sequential(
            nn.Linear(64, action_dim),
            nn.Softmax(dim=-1)
        )
        
        # Critic head
        self.critic = nn.Sequential(
            nn.Linear(64, 1)
        )
        
    def forward(self, x):
        features = self.shared_layers(x)
        action_probs = self.actor(features)
        state_value = self.critic(features)
        return action_probs, state_value
```

The Actor-Critic paridigm became a powerful idea in my mind to learn about, as the promise of being able to maximize the return of an agent in a given environment was too exciting of an idea. Being able to work with graduate students to develop and iterate on robotic agents was the main reason I chose to pursue a career in Deep Learning.

## Transitioning to Industry

After graduating, I joined Topaz Labs where I've been applying deep learning to image enhancement problems. The transition from academic research to industry applications brought new challenges:

1. **Scale**: Working with massive datasets, distributed training, model optimizations, and inference over millions of data points
2. **Efficiency**: Optimizing models for production environments and reducing the cost of running these models
3. **Business Impact**: Translating technical improvements into user value, and ensuring that each engineering decision stems from a highly impactful costumer need.

The main difference between academia and industry is that in academia, you can often get away with using a simple model, a smaller dataset, or an error tolerance that is not feasible in industry. The scrutiny of every output becomes apparent in industry, whereas in acadamia your focus is to push the upper bound of what is possible, not maximize the lower bound of what is acceptable. In acedamic research, I find that the most important thing is to be able to quickly iterate on ideas. In industry, you need to be able to build systems that are robust and scalable, hence the importance of the first two points.

## Current Projects and Research Interests

My current focus is on multi-modal learning and representation learning. I'm particularly interested in how we can build models that understand both the visual and semantic content of media, enabling more intelligent processing and enhancement. At Topaz Labs, my work stems from the need to properly understand the content we are operating on and how to maximaly enhance that content. 

Outside of Topaz, I've been working with Hirebase as the core AI Engineer. Here, I've been expanding whats possible with some of the most extensive up-to-date job data available in one place. 

At both Topaz and Hirebase, alot of my work stems from maximally understanding content and queries so that we can build systems that are able to reason about the data they are operating on. Additionally, these systems need to be efficent which is a constant challenge when improving on existing Deep Learning systems. 

## We build on the shoulders of giants

Software has been the most sought-after skill in the job market since the dot-com boom, and for good reason. Yet what truly distinguishes today's software paradigm is our newfound ability to build systems that can reason about data in profoundly complex ways. While classical systems excelled by efficiently tracking and processing information — making them vastly superior to manual human workflows — the new paradigm fundamentally transforms what's possible.

When I reflect on legacy systems, I think of electronic health records that digitized patient management—tools that essentially made paperwork more manageable and workflows more efficient. These represented a significant but incremental improvement over previous methods.

Today's paradigm, however, shifts the entire foundation. Rather than making paperwork easier, we're enabling 150x more accurate medical diagnoses. Where old systems made us more organized and efficient, new systems actively improve our health, welfare, and quality of life. This distinction is crucial for understanding the true impact of our work and for guiding future development.

That said, the software of the New couldn't exist without the software of the Old, this goes back to the old saying "we build on the shoulders of giants". Without robust data tracking systems and highly optimized pipelines, we wouldn't have the foundation necessary for complex reasoning about data. This realization has been grounding for me—despite my passion for developing cutting-edge models and exploring state-of-the-art machine learning problems, everything ultimately connects back to fundamental software principles.

For some, this connection might seem obvious, but for me, it served as an important reality check — a way to discern what truly matters. To meaningfully advance AI, we must thoroughly understand software's past and present. In my little amount of experience working with those engineers who deeply understand this connection has been deeply educational and helpful to my own growth and understanding of the field. 

I believe the future of AI is in systems that can understand multiple forms of data and reason about their relationships. As I continue my career, I hope to contribute to making such systems a reality.

Stay tuned for more posts about my projects and research! 