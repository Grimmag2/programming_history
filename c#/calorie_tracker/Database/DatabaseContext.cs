//Authors: Ondřej Dacík
using ITUvec.Models;
using Microsoft.EntityFrameworkCore;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ITUvec.Database
{
    class DatabaseContext: DbContext
    {
        private static bool created = false;

        public DbSet<MealModel> Meals => Set<MealModel>();
        public DbSet<FoodModel> Foods => Set<FoodModel>();
        public DbSet<DateModel> Dates => Set<DateModel>();
        public DbSet<MealToFoodModel> MealToFood => Set<MealToFoodModel>();
        public DbSet<UserModel> User => Set<UserModel>();
        public DbSet<AchievementsModel> Achievements=> Set<AchievementsModel>();

        public DatabaseContext()
        {
            //creates a db context, executes only once
            if (!created)
            {
                created = true;
                //Database.EnsureDeleted();
                Database.EnsureCreated();
            }
            
        }

        //specify database file name
        protected override void OnConfiguring(DbContextOptionsBuilder optionsBuilder)
        {
            optionsBuilder.UseSqlite("Data Source=itu.db");
        }

        //create database relations
        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            base.OnModelCreating(modelBuilder);

            //dateModel 1 <-> * mealModel
            modelBuilder.Entity<DateModel>()
                .HasMany<MealModel>(p => p.Meals)
                .WithOne(a => a.Date)
                .HasForeignKey(a => a.DateModelId);

            modelBuilder.Entity<MealModel>()
                .HasOne<DateModel>(a => a.Date)
                .WithMany(p => p.Meals)
                .HasForeignKey(a => a.DateModelId);


            //mealModel * <-> * foodModel
            modelBuilder.Entity<MealToFoodModel>()
                .HasOne<MealModel>(pt => pt.Meal)
                .WithMany(p => p.MealToFood)
                .HasForeignKey(pt => pt.MealId);

            modelBuilder.Entity<MealToFoodModel>()
                .HasOne<FoodModel>(pt => pt.Food)
                .WithMany(t => t.MealToFood)
                .HasForeignKey(pt => pt.FoodId);

            //seed demo data
            Seeds.Seed(modelBuilder);
        }
    }
}
