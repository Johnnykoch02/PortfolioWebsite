# Lessons Learned from Founding an AI Startup

*Posted: September 5, 2024*

## The Founding Journey

When my co-founders and I started Joby AI, we were driven by a simple mission: help job seekers find opportunities that truly match their skills and aspirations. Having experienced the frustration of traditional job platforms firsthand, we believed AI could create a more personalized, efficient job search experience.

## Building the Technical Foundation

Our first technical challenge was creating a system that could understand both job listings and candidate profiles at a deeper level than keyword matching. We experimented with several approaches:

1. **Embedding-based matching**: Using contrastive learning to map jobs and candidates to a shared space
2. **Fine-tuned language models**: Customizing pre-trained models for domain-specific understanding
3. **Retrieval systems**: Combining semantic search with structured data filtering

Ten years ago, the rate of progress was slow enough that achieving cross-modal matching of different data types would be significantly difficult. Today, the most powerful LLMs are capable of generating synthetic data that is indistinguishable from real data, and data collection becomes orders of magnitude easier to do. In order to develop a competitive model, we utilized the following techniques:

- Scraped and normalized job listings from diverse sources
- Synthetically generated job descriptions and candidate profiles that were indistinguishable from real data
- Ran contrastive learning on the data to learn a semantic space where the job descriptions and candidate profiles could be easily matched, and bootstrapped the training of the model on more data which we can induce our biases into on based on assumptions we can make and programatic mapping rules that we can use seperate the space on any axis we want.
- Deploy this model as a product via our API to a wide range of users with various different use cases; from job seekers to HR professionals to recruiters, to different job boards, even to customers who want high quality embeddings for their own applications.

## Scaling Challenges

As we grew from processing a few tens of thousands of jobs to over 180,000k daily, we encountered significant scaling challenges:

```
• Scraping infrastructure needed massive parallelization
• Inference costs threatened to outpace revenue
• Data quality issues amplified with scale
```

Solving these problems was a challenge, but we were able to overcome them by:

- Building a scalable scraping infrastructure that could handle the volume of data we needed to process on the cheapest possible cloud infrastructure we could find
- Implementing, training and deploying the most efficient models for the task at hand which are finetuned on the data we have available so that we can minimize the number of inference errors that we make
- Developing a modular autoscaling framework that can check various community cloud platforms and automatically scale our infrastructure to the amount inference compute we need to run and selecting the cheapest option available, automatically recovering from frequent failures caused by unstable cloud providers, and maximizing the amount of money we save on our infrastructure costs.


## The Business Side

Technical expertise alone wasn't enough. Some key business lessons we learned:

- **Value proposition clarity**: Users didn't care about our sophisticated ML models; they cared about finding better jobs faster.
- **Feedback loops**: Implementing mechanisms to capture user feedback dramatically improved our recommendation quality.
- **Monetization strategy**: Finding the right balance between free features and premium services took multiple iterations.
- **Market education**: Explaining the benefits of AI-driven job matching required simplifying our messaging.

## Looking Forward

The future of AI in recruitment is moving toward ever more personalized experiences. We're now exploring:

- Continue developing insightful LLMs that deeply understand all of our users needs, qualifications, and job preference
- Deploying market intelligence tools to provide API and Web users insightful information about the market and job trends
- Building tools that help match the best candidates and recruiters together based on all of the subtle nuances that our competitors are unable to provide and that we can leverage from our high quality interaction data

## Advice for AI Founders

If you're thinking about starting an AI company, here's my advice:
1. Focus relentlessly on user problems, not technology
2. Build rapid feedback mechanisms into your product
3. Beware of expensive inference costs as you scale
4. Prepare for the "cold start" problem with any recommendation system
5. Don't underestimate the importance of data quality and coverage

Building an AI startup has been challenging but immensely rewarding. I'm excited to see what the future holds for Joby AI and the broader landscape of AI applications! 