class Post < ApplicationRecord
  has_many :comments
  
  validates_presence_of :body, :title, :name
  def self.search(search)
    where("title LIKE ?", "%#{search}%") 
    where("body LIKE ?", "%#{search}%")
    where("mmr LIKE ?", "%#{search}%")
  end
end