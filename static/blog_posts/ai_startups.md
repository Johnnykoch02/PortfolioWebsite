# Joining Joby AI

*Posted: December 15, 2024*

When I started at Joby AI, I was drawn in by a simple mission: help job seekers find opportunities that truly match their skills and aspirations. Having experienced the frustration of traditional job platforms firsthand, the Founder believed AI could create a more personalized, efficient job search experience. As someone with experience in the field, I was excited to be part of a team that was building something new and innovative that users would actually want to use.

## Building the Technical Foundation

1. **Embedding-based matching**: learn representations of our data that can be used to match jobs and candidates
2. **Fine-tuned language models**: customizing pre-trained models for domain-specific understanding that are efficent to run and cost-effective to inference
3. **Retrieval systems**: combining semantic search with structured data filtering

Ten years ago, the rate of progress was slow enough that achieving cross-modal matching of different data types would be significantly difficult. Today, the most powerful LLMs are capable of generating synthetic data that is indistinguishable from real data, and data collection becomes orders of magnitude easier to do. In order to develop a competitive model, in order to be able to compete in this landscape you have to identify what existing platforms are missing and how you can fill that gap. 

For Joby AI, we are focusing on the following:

- Index all of the jobs in the world and make them searchable by natural language queries
- build tools that encourage as many candidates in the world as possible to use our platform to find jobs
- identify missing components of existing job search engines using semantic intelligence offered from the scale of our data and the power of today's current SoTA models.

## Scaling Challenges

As we grew from processing a few tens of thousands of jobs to over 180,000k daily, we encountered significant scaling challenges:

```
• Scraping infrastructure needed massive parallelization
• Inference costs threatened to outpace revenue
• Data quality issues amplified with scale
```

Solving these problems is always a challenge, but we were able to overcome them by continuously iterating on our models and infrastructure to drive the overhead costs down and improve the quality of our data.

## Looking Forward

The future of AI in recruitment is moving toward ever more personalized experiences. We want to be the platform that helps you leverage the technology that exists to bring candidates and recruiters together.

## Advice for others in the semantic search space

1. Focus relentlessly on user problems, not technology
2. Build rapid feedback mechanisms into your product
3. Beware of the overhead costs as you scale your technology, and be willing to always try something new if you're not seeing the returns you want
4. Prepare for the "cold start" problem with any recommendation system, and be willing to iterate on your data collection strategy and model updates to improve your system
5. Don't underestimate the importance of data quality and coverage. Anybody can build a simple recomendation system, but building the best system requires a deep understanding of the problem you're trying to solve and constantly improving the quality of the models your builing.

